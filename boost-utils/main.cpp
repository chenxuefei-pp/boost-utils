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

int main(int argc , char** args)
{
    logger::init_logger("./log", "boost-utils", static_cast<uint32_t>(loglevel::Log_Info));
    
    LOG_DEBUG_F("The value is %s %d %x %f","Hello world",12,12,0.5f);
}
