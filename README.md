# simple-rpc

## 依赖
zmq

## 编译
```
mkdir build
cd build
cmake ..
make
```

## 基本类型的序列化和反序列化

```cpp
#include "serializer.h"
#include <iostream>

//自定义类型需要重载操作符>>和<<
struct foo {
    int n;
    std::string str;
    foo() {}
    foo(int a, std::string s) 
        : n(a), str(s) {}
    friend Serializer& operator>>(Serializer& in, foo& f)
    {
        in >> f.n >> f.str;
        return in;
    }
    friend Serializer& operator<<(Serializer& out, foo& f)
    {
        out << f.n << f.str;
        return out;
    }
};

// 测试原生类型的序列化和反序列化
void test1()
{
    int a = 100;
    std::string c = "hello";
    Serializer sr;
    sr << a;  //序列化
    sr << c;

    int b = 0;
    std::string d;
    sr >> b; //反序列化
    sr >> d;
    std::cout << "b=" << b << ", d=" << d << std::endl;
}

//测试自定义类型的序列化和反序列化
void test2()
{
    foo a(23, "evenleo");
    Serializer sr;
    sr << a;

    foo b;
    sr >> b;
    std::cout << "b.n=" << b.n << ", b.str=" << b.str << std::endl;
}

int main(int argc, char** argv)
{
    test1();
    test2();
    return 0;
}
```
输出结果：
```
b=100, d=hello
b.n=23, b.str=evenleo
```
## RPC
rpc_server
```cpp
#include <string>
#include <iostream>
#include "rpc.h"

// 类成员函数
struct foo {
	int add(int a, int b) { return a + b; }
};

// 普通函数
std::string Strcat(std::string s1, int n) 
{
    return s1 + std::to_string(n);
}

int main()
{
	rpc server;
	server.as_server(5555);
	foo f;
	server.regist("add", &foo::add, &f);  //注册类成员函数
	server.regist("Strcat", Strcat);      //注册普通函数
	server.run();
	return 0;
}
```
rpc_client
```cpp
#include <string>
#include <iostream>
#include "rpc.h"

int main()
{
	rpc client;
	client.as_client("127.0.0.1", 5555);
	std::string str = client.call<std::string>("Strcat", "even", 24).value();
	std::cout << "str=" << str << std::endl;
	int result = client.call<int>("add", 3, 4).value();
	std::cout << "result=" << result << std::endl;
	return 0;
}
```
需要启动rpc_server，然后启动rpc_client，请求Strcat和add返回结果：
```
str=even24
result=7
```

本项目源自GitHub的项目[button-chen/buttonrpc_cpp14](https://github.com/button-chen/buttonrpc_cpp14)，本人只是做了优化，非常感谢原作者