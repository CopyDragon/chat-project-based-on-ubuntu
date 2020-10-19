/*************************************************************************
	> File Name: client.cpp
	> Author: 
	> Mail: 
	> Created Time: Sun Oct 18 18:19:22 2020
 ************************************************************************/

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/shm.h>
#include<iostream>
using namespace std;
int main(){
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("172.29.18.134");
    serv_addr.sin_port = 8000;

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        cout<<"connect() error";

    cout<<" ------------------\n";
    cout<<"|                  |\n";
    cout<<"| 请输入你要的选项:|\n";
    cout<<"|    1:登录        |\n";
    cout<<"|    2:注册        |\n";
    cout<<"|                  |\n";
    cout<<" ------------------ \n\n";
    
    int choice;
    cin>>choice;
    string name,pass,pass1;
    //登录
    if(choice==1){
        while(1){
        cout<<"用户名:";
        cin>>name;
        cout<<"密码:";
        cin>>pass;
        string str="login"+name;
        str+="pass:";
        str+=pass;
        send(sock,str.c_str(),str.length(),0);//发送登录信息
        char buffer[1000];
        memset(buffer,0,sizeof(buffer));
        recv(sock,buffer,sizeof(buffer),0);//接收响应
        string recv_str(buffer);
        if(recv_str=="ok")
            cout<<"登陆成功\n\n";
        else
            cout<<"密码或用户名错误！\n\n";
        }   
    }
    //注册
    else if(choice==2){
        cout<<"注册的用户名:";
        cin>>name;
        while(1){
            cout<<"密码:";
            cin>>pass;
            cout<<"确认密码:";
            cin>>pass1;
            if(pass==pass1)
                break;
            else
                cout<<"两次密码不一致!\n\n";
        }   
        name="name:"+name;
        pass="pass:"+pass;
        string str=name+pass;
        send(sock,str.c_str(),str.length(),0);
        cout<<"注册成功！\n";
    }
    //char sendbuf[1000];
    //while(1){
    //    cin>>sendbuf;
    //    send(sock, sendbuf, strlen(sendbuf),0); 
    //}       
        
}
