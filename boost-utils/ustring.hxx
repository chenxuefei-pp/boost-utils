// **************************************************************************/
//  作者: 陈雪飞<chenxuefei_pp@163.com>
//  日期: 2017年5月5日  14:52:45
//  工程: boost-utils
//  程序: boost-utils
//  文件: ustring.hxx
//  描述: 字节串
// **************************************************************************/
#pragma once

#include "types.hxx"
#include <string>
#include <boost/regex.hpp>

using base_ustring = std::basic_string<uchar>;
using ustringstream = std::basic_stringstream<uchar>;

/**
 * \brief 字节串
 */
class ustring final : public base_ustring {


public:
    ustring() : base_ustring() {}

    /**
     * \brief 通过十六进制字符串构造
     * \param s 十六进制字符串
     */
    explicit ustring(const std::string &s) { from_hexstring(s); }

    /**
     * \brief 初始化sz个0
     * \param sz 个数
     */
    explicit ustring(size_t sz) : base_ustring(sz, '\0') {}

    /**
     * \brief 初始化构造多个_Ch字符
     * \param _Count 个数
     * \param _Ch 字符
     */
    ustring(size_type _Count, uchar _Ch) : base_ustring(_Count, _Ch) {}

    /**
     * \brief 通过容器来构造
     * \tparam _Container 容器类型，判断是否有迭代器来判断
     * \param v 容器
     */
    template<class _Container,
        class = typename std::enable_if<std::_Is_iterator<typename _Container::iterator>::value,
        void>::type>
    explicit ustring(const _Container &v) : base_ustring(v.begin(), v.end()) {}

    /**
     * \brief 通过迭代器构造
     * \tparam _Iter 迭代器类型
     * \param _begin 开始
     * \param _end 结束
     */
    template<class _Iter,
        class = typename std::enable_if<std::_Is_iterator<_Iter>::value,
        void>::type>
    ustring(_Iter _begin, _Iter _end) : base_ustring(_begin, _end) {}

    /**
     * \brief 通过uchar指针数组来构造
     * \param ptr 指针
     * \param _count 需要的长度
     */
    ustring(const uchar *ptr, size_type _count) : base_ustring(ptr, _count) {}

    /**
     * \brief 通过初始化列表来构造（C++11）
     * \param _Ilist 初始化列表
     */
    ustring(std::initializer_list<uchar> _Ilist) : base_ustring(_Ilist) {}

    
    /**
     * \brief 拷贝构造函数
     * \param other 
     */
    ustring(const ustring &other) : base_ustring(other) { }

    /**
     * \brief 右值拷贝 窃取
     * \param other 右值
     */
    ustring(ustring && other) noexcept : base_ustring(std::move(other)) { }

    /**
     * \brief 添加一个uint类型到尾部
     * \param value 要添加的值
     * \return 当前实例
     */
    ustring &append_int(uint value) {
        uchar purebyte[4];
        purebyte[0] = static_cast<uchar>(value >> 24 & 0xFF);
        purebyte[1] = static_cast<uchar>(value >> 16 & 0xFF);
        purebyte[2] = static_cast<uchar>(value >> 8 & 0xFF);
        purebyte[3] = static_cast<uchar>(value & 0xFF);

        append(&purebyte[0], &purebyte[4]);
        return *this;
    }

    /**
     * \brief 添加一个short类型到尾部
     * \param value 值
     * \return 当前实例
     */
    ustring &append_short(ushort value) {
        uchar purebyte[2];
        purebyte[0] = static_cast<uchar>(value >> 8 & 0xFF);
        purebyte[1] = static_cast<uchar>(value & 0xFF);

        append(&purebyte[0], &purebyte[2]);
        return *this;
    }

    /**
     * \brief 弹出一个short值
     * \return 弹出的值
     */
    ushort pop_short() {
        if (this->length() < 2)
            return 0;
        auto r = ustring(this->begin(), this->begin() + 2).to_ushort();
        erase(this->begin(), this->begin() + 2);
        return r;
    }

    /**
     * \brief 弹出一个int类型的值
     * \return 弹出的值
     */
    uint pop_int() {
        if (this->length() < 4)
            return 0;
        auto r = ustring(this->begin(), this->begin() + 4).to_uint();
        erase(this->begin(), this->begin() + 4);
        return r;
    }

    /**
     * \brief 弹出多个字节
     * \param size 需要的字节数
     * \return 返回的字节串
     */
    ustring pop(uint size) {
        if (this->length() < size)
            return ustring();

        auto r = ustring(this->begin(), this->begin() + size);
        this->erase(this->begin(), this->begin() + size);
        return std::move(r);
    }

    /**
     * \brief 转化前两个字符组合成short
     * \return 组合的值
     */
    ushort to_ushort() const {
        if (this->length() < 2)
            return 0;
        ushort data;
        ushort b1 = (*this)[0] << 8;
        ushort b2 = (*this)[1];
        data = b1 | b2;
        return data;
    }

    /**
    * \brief 转化前两个字符组合成int
    * \return 组合的值
    */
    uint to_uint() const {
        if (this->length() < 4)
            return 0;
        uint data;
        uint b1 = (*this)[0] << 24;
        uint b2 = (*this)[1] << 16;
        uint b3 = (*this)[2] << 8;
        uint b4 = (*this)[3];
        data = b1 | b2 | b3 | b4;
        return data;
    }

    
    /**
     * \brief 将字节串转化为十六进制字符串
     * \param split_str 分割符
     * \param is_upper 大小写
     * \return 返回的字符串
     */
    std::string to_hexstring(const char *split_str = nullptr, bool is_upper = true) const {
        std::string str;

        char buffer[4];
        for (auto it = begin(); it != end(); ++it) {
            memset(buffer, 0, 4);
#ifdef WIN32
            sprintf_s(buffer, 4, "%02x", *it);
#else
            sprintf(buffer, "%02x", *it);
#endif
            if (is_upper) {
                str.push_back(static_cast<char>(toupper(buffer[0])));
                str.push_back(static_cast<char>(toupper(buffer[1])));
            }
            else {
                str.push_back(static_cast<char>(tolower(buffer[0])));
                str.push_back(static_cast<char>(tolower(buffer[1])));
            }
            if (split_str)
                str.append(split_str);
        }

        return std::move(str);
    }

    /**
     * \brief 直接将字节串转化为标准字符串
     * \return 标准字符串
     */
    std::string to_stdstring() const {
        return std::move(std::string(reinterpret_cast<const char *>(this->c_str()), this->length()));
    }


    /**
     * \brief 从十六进制字符串转化
     * \param d 十六进制字符串
     */
    void from_hexstring(const std::string &d) {
        const static boost::regex exp("[0-9a-fA-F]{1,2}");   //正则表达式匹配十六进制数
        boost::smatch result;                   //结果集
        uchar b;
        auto start = d.begin();
        auto end = d.end();
        /*遍历查找所有字符串*/
        while (boost::regex_search(start, end, result, exp)) {
            start = result[0].second;
            /*通过字符串生成十六进制数*/
#ifndef WIN32
            b = (uchar)std::stoi(result[0].str().c_str(), 0, 16);
#else
            b = strtol(result[0].str().c_str(), 0, 16);
#endif
            this->push_back(b);
        }
    }


    /**
     * \brief 从十六进制字符串转化
     * \param d 十六进制字符串
     * \return 字节串
     */
    static ustring gen_from_hexstring(const std::string &d) {
        ustring str;
        str.from_hexstring(d);
        return std::move(str);
    }

    /**
     * \brief 生成随机字节串
     * \param sz 需要的长度
     * \return 返回随机字节串
     */
    static ustring gen_random_ustring(size_t sz) {
        ustring data;
        srand(static_cast<unsigned int>(time(nullptr)));
        for (size_t i = 0; i < sz; ++i) {
            auto r = static_cast<uint>(rand() % 255);
            data.push_back(static_cast<uchar>(r));
        }
        return std::move(data);
    }
};

/*基本运算符重载*/
inline ustring &operator<<(ustring &s, uchar c) {
    s.push_back(c);
    return s;
}

inline ustring &operator<<(ustring &s, uint d) {
    s.append_int(d);
    return s;
}

inline ustring &operator<<(ustring &s, ushort d) {
    s.append_short(d);
    return s;
}

inline ustring &operator<<(ustring &s, const ustring &ss) {
    s.append(ss);
    return s;
}

inline ustring &operator<<(ustring &s, const ustring &&ss) {
    s.append(ss);
    return s;
}

inline ustring &operator<<(ustring &s, const std::string d) {
    ustring ss;
    ss.from_hexstring(d);
    s << ss;
    return s;
}

inline std::ostream &operator<<(std::ostream &o, const ustring &s) {
    o << s.to_hexstring(" ");
    return o;
}