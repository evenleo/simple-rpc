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