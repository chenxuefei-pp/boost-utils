// **************************************************************************/
//  作者: 陈雪飞<chenxuefei_pp@163.com>
//  日期: 2017年5月4日  10:43:37
//  工程: boost-utils
//  程序: boost-utils
//  文件: easy_logger.hxx
//  描述: 简易logger
// **************************************************************************/
#pragma once
#include <boost/thread/recursive_mutex.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

/**
 * \brief 简易logger
 */
class   easy_logger
{
public:
    /**
     * \brief log 级别
     */
    using log_level = enum { info, debug, fault };
    using _RecursiveMutex = boost::recursive_mutex;

    /**
     * \brief 简易log方法
     * \tparam Params 参数类型
     * \param level log级别
     * \param params 参数列表
     */
    template<typename... Params>
    static void  log(log_level level, Params... params)
    {
        if(level >= get_logger()->get_level())
            get_logger()->log_(level , header() , params...);
    }

    /**
     * \brief 设定日志级别
     * \param l 级别
     */
    static  void    set_level(log_level l)
    {
        get_logger()->set_level_(l);
    }

protected:

    /**
     * \brief 获取单例logger
     * \return 单例logger实例
     */
    static easy_logger* get_logger()
    {
        static easy_logger _inst;
        return &_inst;
    }

    /**
     * \brief 添加头
     * \return 返回头字符串
     */
    static std::string header()
    {
        return std::move(
            std::string("[") + to_iso_extended_string(boost::posix_time::second_clock::local_time())
            + "]:"
        );
    }

    /**
     * \brief 记录日志实际方法 最终递归函数
     * \tparam Type 参数类型
     * \param value 参数
     */
    template<typename Type>
    void log_(log_level , const Type &value)
    {
        std::cout << value << std::endl;
    }

    /**
     * \brief 记录日志实际方法，通用递归函数
     * \tparam Type 参数类型
     * \tparam Params 参数列表类型
     * \param level 日志级别
     * \param value 参数值
     * \param params 参数列表
     */
    template<typename Type, typename... Params>
    void  log_(log_level level,const Type& value, Params... params)
    {
        _RecursiveMutex::scoped_lock lock(mu);
        std::cout << value;
        log_(level, params...);
    }

    /**
     * \brief 设定日志级别
     * \param l 日志级别
     */
    void    set_level_(log_level l)
    {
        m_level = l;
    }

    /**
     * \brief 获取日志级别
     * \return 日志级别
     */
    log_level get_level()const
    {
        return m_level;
    }

    /**
     * \brief 日志级别
     */
    log_level           m_level { info };   
    /**
     * \brief 递归锁
     */
    _RecursiveMutex     mu;
};

#define EZ_LOG_DEBUG(...) easy_logger::log(easy_logger::debug,__VA_ARGS__)
#define EZ_LOG_INFO(...) easy_logger::log(easy_logger::info,__VA_ARGS__)
#define EZ_LOG_FAULT(...) easy_logger::log(easy_logger::fault,__VA_ARGS__)