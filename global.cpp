/*************************************************************************
	> File Name: global.cpp
	> Author: fuyinglong
	> Mail: 838106527@qq.com
	> Created Time: Wed Oct 21 16:44:19 2020
 ************************************************************************/

#include "global.h"

unordered_map<string,int> name_sock_map;
unordered_map<int,set<int>> group_map;
unordered_map<string,string> from_to_map;//key:用户名 value:key的用户想私聊的用户
//time_point<system_clock> begin_clock;
double total_time;//线程池处理任务的总时间
//clock_t begin_clock;//开始时间，用于性能测试
int total_handle;//总处理请求数，用于性能测试
double top_speed;//记录峰值性能
int total_recv_request;//接收到的请求总数，性能测试
int Bloom_Filter_bitmap[1000000];//布隆过滤器所用的bitmap
queue<int> mission_queue;//任务队列
int mission_num;//任务队列中的任务数量
pthread_cond_t mission_cond;//线程池所需的条件变量
pthread_spinlock_t name_mutex;//互斥锁，锁住需要修改name_sock_map的临界区
pthread_spinlock_t from_mutex;//互斥锁，锁住修改from_to_map的临界区
pthread_spinlock_t group_mutex;//互斥锁，锁住修改group_map的临界区
pthread_mutex_t queue_mutex;//互斥锁，锁住修改任务队列的临界区
int epollfd;
pthread_spinlock_t count_mutex;






