/*************************************************************************
	> File Name: HandleClient.h
	> Author: fuyinglong
	> Mail: 838106527@qq.com
	> Created Time: Wed Oct 21 15:20:29 2020
 ************************************************************************/

#ifndef _HANDLECLIENT_H
#define _HANDLECLIENT_H

#include<iostream>
#include<pthread.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<mysql/mysql.h>
#include<netinet/in.h>
using namespace std;

//线程执行此函数来发送消息
void *handle_send(void *arg);

//线程执行此函数来接收消息
void *handle_recv(void *arg);


#endif
