/*************************************************************************
	> File Name: HandleServerV2.h
	> Author: 
	> Mail: 
	> Created Time: Tue Dec 22 14:23:07 2020
 ************************************************************************/

#ifndef _HANDLESERVERV2_H
#define _HANDLESERVERV2_H

#include<iostream>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<mysql/mysql.h>
#include<time.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<sys/epoll.h>
#include <hiredis/hiredis.h>
using namespace std;

//线程执行此函数，处理请求
//void handle_all_request(string,int);
void* handle_all_request(void *arg);

#endif
