/*************************************************************************
	> File Name: server.cpp
	> Author: 
	> Mail: 
	> Created Time: Sun Oct 18 16:06:56 2020
 ************************************************************************/

#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include <arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<pthread.h>
using namespace std;

int main(){
    int serv_sock, clnt_sock;
    sockaddr_in serv_adr, clnt_adr;
    socklen_t clnt_adr_sz;
    pthread_t t_id;
    pthread_mutex_t mutx;

    pthread_mutex_init(&mutx, NULL); //创建互斥锁
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);//PF_INET:tcp/ip协议   SOCK_STREAM: tcp    0:默认协议
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;//使用地址族
    serv_adr.sin_addr.s_addr = inet_addr("172.29.18.134");//ip地址
    serv_adr.sin_port = 8000;//端口号

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
         cout<<"bind() error";
    if (listen(serv_sock, 5) == -1)//连接队列大小为5
         cout<<"listen() error";
    
    clnt_adr_sz = sizeof(clnt_adr);
    int conn = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz);
    
    char buffer[1000];
    while(1){
        memset(buffer,0,sizeof(buffer));
        int len = recv(conn, buffer, sizeof(buffer),0);
        cout<<buffer<<endl;
    }   
    
}
