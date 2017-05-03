// **************************************************************************/
//  部门: Gospell研发四部应急广播部
//  作者: 陈雪飞<chenxuefei_pp@163.com>
//  日期: 2017年5月3日  15:55:57
//  工程: boost-utils
//  程序: boost-utils
//  文件: main.cpp
//  描述: 主函数测试
// **************************************************************************/


#include "forever_timer.hxx"
#include <boost/thread.hpp>

void    timer_func(uint sec)
{
    std::cout << "Current " << sec << std::endl;
}

void    io_run(boost::asio::io_service &service)
{
    ERRCODE ec;
    boost::asio::io_service::work work(service);
    service.run(ec);
    std::cout << ec << std::endl;
}


int main(int argc , char** args)
{
    boost::asio::io_service service;
    boost::thread(io_run, boost::ref(service)).detach();
    
    forever_timer timer(service);

    timer.add_doing(5, boost::bind(timer_func, 5));
    boost::this_thread::sleep(boost::posix_time::seconds(8));
    timer.del_doing(5);

    boost::this_thread::sleep(boost::posix_time::seconds(4));

    timer.add_doing(3, boost::bind(timer_func, 3));
    timer.add_doing(3, boost::bind(timer_func, 3));

    boost::this_thread::sleep(boost::posix_time::seconds(100));
}
