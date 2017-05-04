// **************************************************************************/
//  作者: 陈雪飞<chenxuefei_pp@163.com>
//  日期: 2017年5月4日  16:28:01
//  工程: boost-utils
//  程序: boost-utils
//  文件: logger.cpp
//  描述: 完整 boost logger 封装
// **************************************************************************/

#include "logger.hxx"
#include <iostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/common.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/expressions/keyword.hpp>

#include <boost/log/attributes.hpp>
#include <boost/log/attributes/timer.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/async_frontend.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/core/null_deleter.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/filesystem.hpp>

//Logger 全局 唯一实例
boost::log::sources::severity_logger_mt<loglevel>   logger::_logger;

namespace logging = boost::log;
namespace attrs = boost::log::attributes;
namespace src = boost::log::sources;
namespace sinks = boost::log::sinks;
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;

BOOST_LOG_ATTRIBUTE_KEYWORD(log_level, "Severity", loglevel)
BOOST_LOG_ATTRIBUTE_KEYWORD(log_timestamp, "TimeStamp", boost::posix_time::ptime)
BOOST_LOG_ATTRIBUTE_KEYWORD(log_uptime, "Uptime", attrs::timer::value_type)
BOOST_LOG_ATTRIBUTE_KEYWORD(log_scope, "Scope", attrs::named_scope::value_type)

/**
 * \brief 初始化日志
 * \param log_dir 文件输出日志
 * \param level 过滤的日志等级
 */
void logger::init_logger(const std::string &log_dir, const std::string &file_prefix, uint32_t level)
{
    //判断是否可以创建文件夹
    auto log_dir_path = boost::filesystem::path(log_dir);
    if (!boost::filesystem::exists(log_dir_path))
    {
        if (!boost::filesystem::create_directory(log_dir_path))
        {
            std::cerr << "无法创建日志文件夹" << log_dir << " 请确认是否有写权限" << std::endl;
            exit(-2);
        }
    }
    //日志处理格式
    logging::formatter formatter =
        expr::stream
        << "[" << expr::format_date_time(log_timestamp, "%H:%M:%S") //日志记录时间
        << "]" << expr::if_(expr::has_attr(log_uptime))             //如果有上一条记录到这条记录的路径时间,则输出
        [
            expr::stream << "[" << format_date_time(log_uptime, "%O:%M:%S") << "]"
        ]
    << "[" << log_level << "]"      //对日志记录进行精细划分
        << expr::if_(expr::is_in_range(log_level, loglevel::Log_Info, loglevel::Log_Debug))   //如果日志等级大于Debug等级则记录下具体的函数作用域,更方便调试
        [
            expr::stream << expr::format_named_scope(log_scope, keywords::format = "[%n]")
        ]
    << expr::if_(expr::is_in_range(log_level, loglevel::Log_Debug, loglevel::Log_Hell))
        [
            expr::stream << expr::format_named_scope(log_scope, keywords::format = "[%n](%f:%l)")
        ]
    << ":" << expr::message;

    //增加文件日志输出功能，记录成文件格式，方便前台解析
    auto file_sink = boost::make_shared<logging::sinks::asynchronous_sink<logging::sinks::text_file_backend> >
        (
            keywords::file_name = log_dir + "/" + file_prefix + "-%Y%m%d-%N.log",   //文件名
            keywords::rotation_size = 10 * 1024 * 1024,                             //单个文件限制大小
            keywords::open_mode = std::ios::app,                                    //往后增加模式
            keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0)    //每天重建
            );
    //控制台日志输出
    auto console_sink = boost::make_shared<logging::sinks::asynchronous_sink<logging::sinks::text_ostream_backend> >
        ();
    /*设定Console LOG*/
    {
        console_sink->locked_backend()->add_stream
        (boost::shared_ptr<std::ostream>{&std::cout, boost::null_deleter()});
        console_sink->locked_backend()->auto_flush(true);
    }

    {
        file_sink->locked_backend()->set_file_collector(logging::sinks::file::make_collector
        (
            keywords::target = log_dir,                    //文件夹名
            keywords::max_size = 50 * 1024 * 1024,          //文件夹所占最大空间
            keywords::min_free_space = 100 * 1024 * 1024    //磁盘最小预留空间
        ));
        file_sink->locked_backend()->auto_flush(true);
        file_sink->locked_backend()->scan_for_files();      //查找日志同名文件
    }

    logging::add_common_attributes();

    file_sink->set_filter(log_level >= static_cast<loglevel>(level));   //文件设定日志等级
    console_sink->set_filter(log_level >= static_cast<loglevel>(level)); //控制台设定日志等级

    file_sink->set_formatter(formatter);
    console_sink->set_formatter(formatter);

    logging::core::get()->add_thread_attribute("Uptime", attrs::timer());
    logging::core::get()->add_global_attribute("Scope", attrs::named_scope());
    logging::core::get()->add_sink(file_sink);
    logging::core::get()->add_sink(console_sink);
}