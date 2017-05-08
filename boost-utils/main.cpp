// **************************************************************************/
//  部门: Gospell研发四部应急广播部
//  作者: 陈雪飞<chenxuefei_pp@163.com>
//  日期: 2017年5月3日  15:55:57
//  工程: boost-utils
//  程序: boost-utils
//  文件: main.cpp
//  描述: 主函数测试
// **************************************************************************/

#include "logger/logger.hxx"
#include <iostream>
#include "ustring.hxx"
#include <boost/timer.hpp>
#include <boost/log/attributes/current_thread_id.hpp>
#include <boost/thread/v2/thread.hpp>

int main(int argc , char** args)
{
    
    logger::init_logger("./log", "boost-utils", static_cast<uint32_t>(loglevel::Log_Info));
    uchar buf[256] = { 1 };
    uchar dest[256] = { 0 };
    ustring ubuf(256,1);

    boost::timer timer;
    {
        memcpy(dest, buf, 256);   
        LOG_DEBUG_F("memcpy() time : %f", timer.elapsed());
    }
    timer.restart();
    
    ustring data(ubuf);
    LOG_DEBUG_F("copy() time : %f", timer.elapsed());
    
    boost::this_thread::sleep(boost::posix_time::seconds(10000));

}
