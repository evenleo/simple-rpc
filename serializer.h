#ifndef _SERIALIZER_H_
#define _SERIALIZER_H_

#include <assert.h>
#include <string.h>
#include <vector>
#include <algorithm>
#include <memory>
#include <initializer_list>

// 存储数据流的容器
class Buffer : public std::vector<char> {
public:
    typedef std::shared_ptr<Buffer> ptr;
    Buffer() : curpos_(0) {}
    Buffer(const char* s, size_t len) : curpos_(0)
    {
        insert(begin(), s, s + len);
    }
    const char* data() { return &(*this)[0]; }
    const char* curdata() { return &(*this)[curpos_]; }
    size_t cursize() const { return size() - curpos_; }
    void offset(int k) { curpos_ += k; }
    void append(const char* s, size_t len) { insert(end(), s, s + len); }
    void reset() { 
        curpos_ = 0;
        clear();
    }

private:
    size_t curpos_;
};

// 主机字节序是否小端字节序
static bool isLittleEndian()
{
    static uint16_t flag = 1;
    static bool little_end_flag = *((uint8_t*)&flag) == 1;
    return little_end_flag;
}

class Serializer {
public:
    typedef std::shared_ptr<Serializer> ptr;
    Serializer() { buffer_ = std::make_shared<Buffer>(); }
    Serializer(const char* s, size_t len)
    {
        buffer_ = std::make_shared<Buffer>();
        input(s, len);
    }
    Serializer(Buffer::ptr buffer)
        : buffer_(buffer)
    {
    }

    template <typename T>
    void input_type(T t);

    template <typename T>
    void output_type(T& t);

    void reset() { buffer_->reset(); }
    void clear() { reset(); }
    void input(const char* data, int len) { buffer_->append(data, len); }

    template <typename Tuple, std::size_t Id>
    void getv(Serializer& ds, Tuple& t)
    {
        ds >> std::get<Id>(t);
    }

    template <typename Tuple, std::size_t... I>
    Tuple get_tuple(std::index_sequence<I...>)
    {
        Tuple t;
        std::initializer_list<int> { (getv<Tuple, I>(*this, t), 0)... };
        return t;
    }

    template <typename T>
    Serializer& operator>>(T& i)
    {
        output_type(i);
        return *this;
    }

    template <typename T>
    Serializer& operator<<(T i)
    {
        input_type(i);
        return *this;
    }

    const char* data() { return buffer_->curdata(); }
    size_t size() const { return buffer_->cursize(); }
    std::string toString()
    {
        return std::string(data(), size());
    }

private:
    static void byteOrder(char* s, int len)
    {
        if (isLittleEndian())
            std::reverse(s, s + len);
    }

private:
    int byteOrder_;
    Buffer::ptr buffer_;
};

template <typename T>
inline void Serializer::input_type(T v)
{
    size_t len = sizeof(v);
    char* p = reinterpret_cast<char*>(&v);
    byteOrder(p, len);
    input(p, len);
}

// 偏特化
template <>
inline void Serializer::input_type(std::string v)
{
    // 先存入字符串长度
    uint16_t len = v.size();
    input_type(len);
    byteOrder(const_cast<char*>(v.c_str()), len);
    // 再存入字符串
    input(v.c_str(), len);
}

template <>
inline void Serializer::input_type(const char* v)
{
    input_type<std::string>(std::string(v));
}

template <typename T>
inline void Serializer::output_type(T& v)
{
    size_t len = sizeof(v);
    assert(size() >= len);
    ::memcpy(&v, data(), len);
    buffer_->offset(len);
    byteOrder(reinterpret_cast<char*>(&v), len);
}

// 偏特化
template <>
inline void Serializer::output_type(std::string& v)
{
    uint16_t strLen = 0;
    output_type(strLen);
    v = std::string(data(), strLen);
    buffer_->offset(strLen);
    byteOrder(const_cast<char*>(v.c_str()), strLen);
}

#endif