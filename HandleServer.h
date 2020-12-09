/*************************************************************************
	> File Name: handle_server.h
	> Author: fuyinglong
	> Mail: 838106527@qq.com
	> Created Time: Tue Oct 20 16:04:11 2020
 ************************************************************************/

#ifndef _HANDLE_SERVER_H
#define _HANDLE_SERVER_H

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
#include <hiredis/hiredis.h>
using namespace std;

//线程执行此函数，处理请求
void *handle_all_request(void *arg);


#endif
