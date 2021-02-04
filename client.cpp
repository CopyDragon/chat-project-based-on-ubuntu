/*************************************************************************
	> File Name: client.cpp
	> Author: fuyinglong
	> Mail: 838106527@qq.com
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
#include<fstream>
#include<iostream>
#include "HandleClient.h"
using namespace std;

extern void *hand_recv(void *arg);

extern void *handle_send(void *arg);

int main(){
    int sock;
    struct sockaddr_in serv_addr;
    pthread_t snd_thread, rcv_thread;
    sock = socket(PF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr("192.168.3.202");
    serv_addr.sin_port = htons(8023);

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) == -1)
        cout<<"connect() error";
    
    int choice;
    string name,pass,pass1;
    bool if_login=false;//记录是否登录成功
    string login_name;//记录成功登录的用户名

    //2020.12.9新增：发送本地cookie，并接收服务器答复，如果答复通过就不用登录
    //先检查是否存在cookie文件
    ifstream f("cookie.txt");
    string cookie_str;
    if(f.good()){
        f>>cookie_str;
        f.close();
        cookie_str="cookie:"+cookie_str;
        //将cookie发送到服务器
        send(sock,cookie_str.c_str(),cookie_str.length()+1,0);
        //接收服务器答复
        char cookie_ans[100];
        memset(cookie_ans,0,sizeof(cookie_ans));
        recv(sock,cookie_ans,sizeof(cookie_ans),0);
        //判断服务器答复是否通过
        string ans_str(cookie_ans);
        if(ans_str!="NULL"){//redis查询到了cookie，通过
            if_login=true;
            login_name=ans_str;
        }
    }    
    if(!if_login){
        cout<<" ------------------\n";
        cout<<"|                  |\n";
        cout<<"| 请输入你要的选项:|\n";
        cout<<"|    0:退出        |\n";
        cout<<"|    1:登录        |\n";
        cout<<"|    2:注册        |\n";
        cout<<"|                  |\n";
        cout<<" ------------------ \n\n";
    }

    //开始处理各种事务
    while(1){
        if(if_login)
            break;
        cin>>choice;
        if(choice==0)
            break;
        //登录
        else if(choice==1&&!if_login){
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
                if(recv_str.substr(0,2)=="ok"){
                    if_login=true;
                    login_name=name;

                    //2020.12.9新增：本地建立cookie文件保存sessionid
                    string tmpstr=recv_str.substr(2);
                    tmpstr="cat > cookie.txt <<end \n"+tmpstr+"\nend";
                    system(tmpstr.c_str());

                    cout<<"登陆成功\n\n";
                    break;
                }
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
            cout<<"\n继续输入你要的选项:";     
        }
        //char sendbuf[1000];
        //while(1){
        //    cin>>sendbuf;
        //    send(sock, sendbuf, strlen(sendbuf),0); 
        //}       
        
        if(if_login)
            break;

    }
    //登陆成功
    while(if_login&&1){
        if(if_login){
            system("clear");
            cout<<"        欢迎回来,"<<login_name<<endl;
            cout<<" -------------------------------------------\n";
            cout<<"|                                           |\n";
            cout<<"|          请选择你要的选项：               |\n";
            cout<<"|              0:退出                       |\n";
            cout<<"|              1:发起单独聊天               |\n";
            cout<<"|              2:发起群聊                   |\n";
            cout<<"|                                           |\n";
            cout<<" ------------------------------------------- \n\n";
        }
        cin>>choice;
        pthread_t send_t,recv_t;//线程ID
        void *thread_return;
        if(choice==0)
            break;
        if(choice==1){
            cout<<"请输入对方的用户名:";
            string target_name,content;
            cin>>target_name;
            string sendstr("target:"+target_name+"from:"+login_name);//标识目标用户+源用户
            send(sock,sendstr.c_str(),sendstr.length(),0);//先向服务器发送目标用户、源用户
            cout<<"请输入你想说的话(输入exit退出)：\n";
            auto send_thread=pthread_create(&send_t,NULL,handle_send,(void *)&sock);//创建发送线程
            auto recv_thread=pthread_create(&recv_t,NULL,handle_recv,(void *)&sock);//创建接收线程
            pthread_join(send_t,&thread_return);
            //pthread_join(recv_t,&thread_return);
            pthread_cancel(recv_t);
        }    
        if(choice==2){
            cout<<"请输入群号:";
            int num;
            cin>>num;
            string sendstr("group:"+to_string(num));
            send(sock,sendstr.c_str(),sendstr.length(),0);
            cout<<"请输入你想说的话(输入exit退出)：\n";
            int sock1=-sock;
            auto send_thread=pthread_create(&send_t,NULL,handle_send,(void *)&sock1);//创建发送线程
            auto recv_thread=pthread_create(&recv_t,NULL,handle_recv,(void *)&sock);//创建接收线程
            pthread_join(send_t,&thread_return);
            pthread_cancel(recv_t);
        }
    } 
    close(sock);
}
