/*************************************************************************
	> File Name: global.h
	> Author: fuyinglong
	> Mail: 838106527@qq.com
	> Created Time: Wed Oct 21 16:41:24 2020
 ************************************************************************/

#ifndef _GLOBAL_H
#define _GLOBAL_H

#include<set>
#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<vector>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<mysql/mysql.h>
#include<unordered_map>
#include<pthread.h>
#include<sys/epoll.h>
#include<fcntl.h>
#include<unistd.h>
#include<chrono>
#include<boost/bind.hpp>
#include<boost/asio.hpp>
#include<errno.h>
using namespace std;
using namespace chrono;

extern unordered_map<string,int> name_sock_map;//记录名字和文件描述符
extern unordered_map<int,set<int>> group_map;//记录群号和对应的文件描述符集合
extern unordered_map<string,string> from_to_map;//key:用户名 value:key的用户想私聊的用户
extern time_point<system_clock> begin_clock;//开始时间，用于压力测试
//extern clock_t begin_clock;//开始时间，用于性能测试，有bug
extern int total_handle;//总处理请求数，用于性能测试
#endif
