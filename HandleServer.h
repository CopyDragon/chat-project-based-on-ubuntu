/*************************************************************************
	> File Name: handle_server.h
	> Author: 
	> Mail: 
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
#include<netinet/in.h>
using namespace std;
class HandleServer{
    public:
        static sockaddr_in clnt_adr;
        HandleServer() {};
        static void *handle_all_request(void *arg);

};

#endif
