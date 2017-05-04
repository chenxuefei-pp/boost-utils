// **************************************************************************/
//  作者: 陈雪飞<chenxuefei_pp@163.com>
//  日期: 2017年5月3日  15:39:37
//  工程: boost-utils
//  程序: boost-utils
//  文件: forever_timer.hxx
//  描述: 循环定时器
// **************************************************************************/
#pragma once

#include <boost/asio/deadline_timer.hpp>
#include <boost/function.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/bind.hpp>
#include "types.hxx"

/**
 * \brief 多任务循环定时器
 */
class   forever_timer
{
public:

    /*基本类型定义*/
    /**
     * \brief 回调函数定义
     */
    using _DoingType = boost::function<void()>;
    /**
     * \brief 基本函数列表定义，一个multimap，时间为key值，回调为value值
     */
    using _FuncListType = std::multimap<uint, _DoingType>;
    /**
     * \brief 互斥量定义
     */
    using _Mutex = boost::mutex;
    /**
     * \brief 定时器定义
     */
    using _DeadlineTimer = boost::asio::deadline_timer;

    /**
     * \brief 调度器定义
     */
    using _IoService = boost::asio::io_service;
    /**
     * \brief 定时器状态
     */
    using _Status = enum {started,stoped};
public:
    /**
     * \brief 从基本调度器构造，构造初始不启动计时器
     * \param io_service 调度器
     */
    explicit forever_timer(_IoService& io_service)
        : timer(io_service)
    {
    }

    void    add_doing(uint sec, const _DoingType &d)
    {
        _Mutex::scoped_lock lock(mu);
        doing_list.insert({ sec,d });
        /*当调度器处于停止状态，加入任务自动开始计时器*/
        if (status == stoped)
            start();
    }

    /**
     * \brief 删除计时，当前只能将时间内的任务全部删除，无法删除单任务
     * \param sec 要删除的定时时间
     */
    void    del_doing(uint sec)
    {
        _Mutex::scoped_lock lock(mu);
        doing_list.erase(sec);
    }

protected:

    /**
     * \brief 开始定时器
     */
    void    start()
    {
        auto sec = doing_list.begin()->first;
        timer.expires_from_now(boost::posix_time::seconds(sec));
        timer.async_wait(boost::bind(&forever_timer::handler, this, sec, _1));
        status = started;
    }

    /**
     * \brief 从链表中取得下一个定时
     * \param sec 上一个定时
     * \return 
     */
    _FuncListType::_Pairii           wait_next(uint sec)
    {
        _Mutex::scoped_lock lock(mu);

        if (doing_list.empty())
        {
            status = stoped;
            return{ doing_list.end(),doing_list.end() };
        }

        _FuncListType::iterator next;
        uint next_sec = 0;

        auto it = doing_list.equal_range(sec);
        next = doing_list.upper_bound(sec) == doing_list.end() ? doing_list.begin() : doing_list.upper_bound(sec);

        next_sec = next == doing_list.begin() ? next->first : next->first - it.first->first;
        timer.expires_from_now(boost::posix_time::seconds(next_sec));
        timer.async_wait(boost::bind(&forever_timer::handler, this, next->first, _1));

        return it;
    }

    /**
     * \brief 定时器内置handler
     * \param sec 当前运行到的时间
     * \param ec 错误代码
     */
    void                handler(uint sec, const ERRCODE &ec)
    {
        if (ec)
            return;

        auto cur = wait_next(sec);

        _Mutex::scoped_lock lock(mu);
        if (cur == _FuncListType::_Pairii{ doing_list.end(), doing_list.end() })
            return;

        auto bg = cur.first, ed = cur.second;

        while (bg != ed)
        {
            bg->second();
            ++bg;
        }
    }

private:

    /**
     * \brief 状态代码，初始为停止
     */
    _Status             status{ stoped };
    /**
     * \brief 互斥量，避免多线程访问
     */
    _Mutex              mu;
    /**
     * \brief 主timer
     */
    _DeadlineTimer      timer;
    /**
     * \brief 任务列表
     */
    _FuncListType       doing_list;
};



