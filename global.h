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
#include<queue>
#include<chrono>
#include<boost/bind.hpp>
#include<boost/asio.hpp>
#include<errno.h>
using namespace std;
using namespace chrono;

extern unordered_map<string,int> name_sock_map;//记录名字和文件描述符
extern unordered_map<int,set<int>> group_map;//记录群号和对应的文件描述符集合
extern unordered_map<string,string> from_to_map;//key:用户名 value:key的用户想私聊的用户
//extern time_point<system_clock> begin_clock;//开始时间，用于压力测试
//extern clock_t begin_clock;//开始时间，用于性能测试，有bug
extern double total_time;//线程池处理任务的总时间
extern int total_handle;//总处理请求数，用于性能测试
extern double top_speed;//记录峰值性能
extern int total_recv_request;//接收到的请求总数，性能测试
extern int Bloom_Filter_bitmap[1000000];//布隆过滤器所用的bitmap
extern queue<int> mission_queue;//任务队列
extern int mission_num;//任务队列中的任务数量
extern pthread_cond_t mission_cond;//线程池所需的条件变量
extern pthread_spinlock_t name_mutex;//互斥锁，锁住需要修改name_sock_map的临界区
extern pthread_spinlock_t from_mutex;//互斥锁，锁住修改from_to_map的临界区
extern pthread_spinlock_t group_mutex;//互斥锁，锁住修改group_map的临界区
extern pthread_mutex_t queue_mutex;//互斥锁，锁住修改任务队列的临界区
extern int epollfd;
extern pthread_spinlock_t count_mutex;



#endif
