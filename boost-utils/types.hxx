// **************************************************************************/
//  作者: 陈雪飞<chenxuefei_pp@163.com>
//  日期: 2017年5月3日  15:40:17
//  工程: boost-utils
//  程序: boost-utils
//  文件: types.hxx
//  描述: 基本类型定义
// **************************************************************************/
#pragma once

#include <boost/system/error_code.hpp>

/**
 * \brief BOOST 错误代码
 */
using ERRCODE = boost::system::error_code;

/**
* \brief 基本无符号类型定义
*/
using uchar = unsigned char;
using uint = unsigned int;
using ulong = unsigned long;
using ushort = unsigned short;

#ifdef WIN32
    using       ulong64 = unsigned __int64;
#else
    using       ulong64 = unsigned long long;
#endif

#ifdef  WIN32
    using       word = unsigned __int16;
    using       dword = unsigned __int32;
    using       qword = unsigned __int64;
#else
    using word = ushort;
    using dword = uint;
    using qword = ulong64;
#endif