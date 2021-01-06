/*************************************************************************
	> File Name: serverV2.cpp
	> Author: 
	> Mail: 
	> Created Time: Mon Dec 21 12:27:10 2020
 ************************************************************************/

#include<iostream>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<vector>
#include <arpa/inet.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<mysql/mysql.h>
#include<unordered_map>
#include<pthread.h>
#include <sys/epoll.h>
#include <fcntl.h>  
#include <unistd.h>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include<errno.h>
#include "global.h"
#include "HandleServerV2.h"
using namespace std;

//#define MAXLINE 1000  
//#define OPEN_MAX 100

//listen的backlog大小
#define LISTENQ 20
//监听端口号
#define SERV_PORT 8000
#define INFTIM 1000 

extern void handle_all_request(string epoll_str,int conn_num,int epollfd);
extern unordered_map<string,int> name_sock_map;//记录名字和套接字描述符

//将参数的文件描述符设为非阻塞
void setnonblocking(int sock)  
{  
    int opts;  
    opts=fcntl(sock,F_GETFL);  
    if(opts<0)  
    {  
        perror("fcntl(sock,GETFL)");  
        exit(1);  
    }  
    opts = opts|O_NONBLOCK;  
    if(fcntl(sock,F_SETFL,opts)<0)  
    {  
        perror("fcntl(sock,SETFL,opts)");  
        exit(1);  
    }      
}  

int main(){
    int i, maxi, listenfd, connfd, sockfd,epfd,nfds;  
    ssize_t n;  
    //char line[MAXLINE];  
    socklen_t clilen;  
    //声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件  
    struct epoll_event ev,events[200];  
    //生成用于处理accept的epoll专用的文件描述符  
    epfd=epoll_create(256);  
    struct sockaddr_in clientaddr;  
    struct sockaddr_in serveraddr;  
    listenfd = socket(PF_INET, SOCK_STREAM, 0);  
    //把socket设置为非阻塞方式  
    setnonblocking(listenfd);  
    //设置与要处理的事件相关的文件描述符  
    ev.data.fd=listenfd;  
    //设置要处理的事件类型  
    ev.events=EPOLLIN|EPOLLET;  
    //注册epoll事件  
    epoll_ctl(epfd,EPOLL_CTL_ADD,listenfd,&ev); //相当于Select模型的FD_SET  
    //设置serveraddr
    bzero(&serveraddr, sizeof(serveraddr));  
    serveraddr.sin_family = AF_INET;  
    serveraddr.sin_addr.s_addr = inet_addr("172.29.18.134");//此处设为服务器的ip
    serveraddr.sin_port=8000;  
    bind(listenfd,(sockaddr *)&serveraddr, sizeof(serveraddr));  
    listen(listenfd, LISTENQ); 
    clilen=sizeof(clientaddr);
    maxi = 0;   

    /* 定义一个10线程的线程池 */
    boost::asio::thread_pool tp(10);

    while(1){  
        cout<<"--------------------------"<<endl;
        cout<<"epoll_wait阻塞中"<<endl;
        //等待epoll事件的发生  
        nfds=epoll_wait(epfd,events,200,-1);//最后一个参数是timeout，0:立即返回，-1:一直阻塞直到有事件，x:等待x毫秒
        cout<<"epoll_wait返回，有事件发生"<<endl;
        //处理所发生的所有事件  
        for(i=0;i<nfds;++i)  
        {  
            //有新客户端连接服务器
            if(events[i].data.fd==listenfd) 
            {  
                connfd = accept(listenfd,(sockaddr *)&clientaddr, &clilen);  
                if(connfd<0){  
                     perror("connfd<0");  
                     exit(1);  
                }  
                else{
                    cout<<"用户"<<inet_ntoa(clientaddr.sin_addr)<<"正在连接\n";
                }
                //设置用于读操作的文件描述符  
                ev.data.fd=connfd;  
                //设置用于注册的读操作事件，采用ET边缘触发，为防止多个线程处理同一socket而使用EPOLLONESHOT  
                ev.events=EPOLLIN|EPOLLET|EPOLLONESHOT;
                //边缘触发要将套接字设为非阻塞
                setnonblocking(connfd);
                //注册ev  
                epoll_ctl(epfd,EPOLL_CTL_ADD,connfd,&ev);  
            }  
            //接收到读事件
            else if(events[i].events&EPOLLIN)  
            {  
                sockfd = events[i].data.fd;
                cout<<"接收到读事件"<<endl;
                string recv_str;
                /*这段代码不会被重复触发，所以我们循环读取数据，以确保把socket缓冲区的数据全部读取*/
                while(1){
                    char buf[10];
                    memset(buf, 0, sizeof(buf));
                    int ret  = recv(sockfd, buf, sizeof(buf), 0);
                    if(ret < 0){
                        cout<<"recv返回值小于0"<<endl;
                        //对于非阻塞IO，下面的事件成立标识数据已经全部读取完毕。
                        if((errno == EAGAIN) || (errno == EWOULDBLOCK)){
                            printf("数据读取完毕\n");
                            cout<<"接收到的完整内容为："<<recv_str<<endl;
                            cout<<"开始用线程池处理事件"<<endl;
                            boost::asio::post(boost::bind(handle_all_request,recv_str,sockfd,epfd)); //处理事件
                            break;
                        }
                        cout<<"errno:"<<errno<<endl;
                        close(sockfd);
                        events[i].data.fd=-1;
                        break;                       
                    }
                    else if(ret == 0){
                        cout<<"recv返回值为0"<<endl;
                        close(sockfd); 
                        events[i].data.fd=-1;
                    }
                    else{
                        printf("接收到内容如下: %s\n",buf); 
                        string tmp(buf);
                        recv_str+=tmp;
                    }
                }
            }  
        } 
    }  
    close(listenfd);
}
