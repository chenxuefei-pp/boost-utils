// **************************************************************************/
//  作者: 陈雪飞<chenxuefei_pp@163.com>
//  日期: 2017年5月4日  16:27:08
//  工程: boost-utils
//  程序: boost-utils
//  文件: logger.hxx
//  描述: 完整boost logger封装
// **************************************************************************/
#pragma once

#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/attributes/named_scope.hpp>
#include <boost/format.hpp>


/**
 * \brief 日志等级
 */
enum class loglevel
{
    Log_Info,
    Log_Notice,
    Log_Debug,
    Log_Warning,
    Log_Error,
    Log_Fatal,
    Log_Hell
};

/**
    * \brief 日志等级输出重载
    * \tparam CharT
    * \tparam TraitsT
    * \param strm 待输出的流
    * \param lvl 日志等级
    * \return
    */
template< typename CharT, typename TraitsT >
std::basic_ostream< CharT, TraitsT >& operator << (
    std::basic_ostream< CharT, TraitsT >& strm, loglevel lvl)
{
    static const char* const level_str[] =
    {
        "Info",
        "Notice",
        "Debug",
        "Warning",
        "Error",
        "Fatal",
        "Hell"
    };
    if (static_cast<std::size_t>(lvl) < (sizeof(level_str) / sizeof(*level_str)))
        strm << level_str[static_cast<int>(lvl)];
    else
        strm << static_cast<int>(lvl);
    return strm;
}

/**
    * \brief 日志等级比较
    * \param l1
    * \param l2
    * \return
    */
inline  bool   operator < (loglevel &l1, loglevel & l2)
{
    return static_cast<std::size_t>(l1) < static_cast<std::size_t>(l2);
}

/**
* \brief logger类，初始化需要
* \使用方法 LOG_{DEBUG|INFO|WARNING|ERROR|NOTICE|FATAL|FORMAT}(message)
*/
class   logger    final
{
public:

    /**
     * \brief 初始化日志
     * \param log_dir 日志目录
     * \param file_prefix 日志文件前缀
     * \param level 日志过滤等级
     */
    static  void    init_logger(const std::string &log_dir,const std::string &file_prefix, uint32_t level);

    /**
     * \brief 格式化函数 采用C++11变长模板
     * \tparam Params 模板参数类型
     * \param fmt 格式化字符串
     * \param params 格式化参数列表
     * \return 格式化好的字符串
     */
    template<typename ...Params>
    static  std::string  format(const std::string &fmt , Params... params) 
    {
        auto formatter = boost::format(fmt);
        return std::move(_log_format(formatter, params...).str());
    }

    /**
     * \brief logger实例类型
     */
    static      boost::log::sources::severity_logger_mt<loglevel>     _logger;

protected:

    /**
     * \brief 最终格式化递归
     * \tparam Type 格式化类型
     * \param fmt 格式化实例
     * \param value 格式化值
     * \return 格式化实例
     */
    template<typename Type>
    static  boost::format &  _log_format(boost::format &fmt, const Type& value)
    {
        return fmt % value;
    }

    /**
     * \brief 格式化递归
     * \tparam Type 格式化类型
     * \tparam Params 格式化参数列表
     * \param fmt 格式化实例
     * \param value 格式化unpack值
     * \param params 格式化下个参数列表
     * \return 格式化实例
     */
    template<typename Type, typename ...Params>
    static  boost::format &  _log_format(boost::format &fmt, const Type& value, Params... params)
    {
        return _log_format((fmt % value), params...);
    }
};

/**
* \brief Log宏
* \param lvl
* \param Message
*/
#define         LOG_LEVEL(lvl,Message)  {BOOST_LOG_FUNC();BOOST_LOG_SEV(logger::_logger,loglevel::lvl)<<Message;}
/**
* \brief 日志信息等级宏
* \param Message
*/
#define         LOG_INFO(Message)       LOG_LEVEL(Log_Info,Message)
/**
* \brief 日志提示等级宏
* \param Message
*/
#define         LOG_NOTICE(Message)     LOG_LEVEL(Log_Notice,Message)
/**
* \brief 日志调试等级宏
* \param Message
*/
#define         LOG_DEBUG(Message)      LOG_LEVEL(Log_Debug,Message)
/**
* \brief 日志警告等级宏
* \param Message
*/
#define         LOG_WARNING(Message)    LOG_LEVEL(Log_Warning,Message)
/**
* \brief 日志错误等级宏
* \param Message
*/
#define         LOG_ERROR(Message)      LOG_LEVEL(Log_Error,Message)
/**
* \brief 日志崩溃等级宏
* \param Message
*/
#define         LOG_FATAL(Message)      LOG_LEVEL(Log_Fatal,Message)


/**
 * \brief 格式化日志
 * \param fmt 待格式化字符串
 */
#define         LOG_FORMAT(fmt,lvl, ...)    LOG_LEVEL(lvl, logger::format(fmt,__VA_ARGS__) )
 /**
 * \brief 格式化日志信息等级宏
 * \param Message
 */
#define         LOG_INFO_F(fmt ,...)       LOG_FORMAT(fmt,Log_Info,__VA_ARGS__)
 /**
 * \brief 格式化日志提示等级宏
 * \param Message
 */
#define         LOG_NOTICE_F(fmt ,...)     LOG_FORMAT(fmt,Log_Notice,__VA_ARGS__)
 /**
 * \brief 格式化日志调试等级宏
 * \param Message
 */
#define         LOG_DEBUG_F(fmt ,...)      LOG_FORMAT(fmt,Log_Debug,__VA_ARGS__)
 /**
 * \brief 格式化日志警告等级宏
 * \param Message
 */
#define         LOG_WARNING_F(fmt ,...)    LOG_FORMAT(fmt,Log_Warning,__VA_ARGS__)
 /**
 * \brief 格式化日志错误等级宏
 * \param Message
 */
#define         LOG_ERROR_F(fmt ,...)      LOG_FORMAT(fmt,Log_Error,__VA_ARGS__)
 /**
 * \brief 格式化日志崩溃等级宏
 * \param Message
 */
#define         LOG_FATAL_F(fmt ,...)      LOG_FORMAT(fmt,Log_Fatal,__VA_ARGS__)