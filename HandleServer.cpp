/*************************************************************************
	> File Name: handle_server.cpp
	> Author: 
	> Mail: 
	> Created Time: Tue Oct 20 16:11:22 2020
 ************************************************************************/

#include"HandleServer.h"
#include"global.h"

//extern unordered_map<pair<int,string>,pair<int,string>> from_to_map;//记录目标用户、源用户
extern unordered_map<string,int> name_sock_map;//名字和套接字描述符

void* handle_all_request(void *arg){
    int conn=*(int *)arg;
    int target_conn;
    char buffer[1000];
    string name,pass;
    bool if_login=false;//记录当前服务对象是否成功登录
    string login_name;//记录当前服务对象的名字
    MYSQL *con=mysql_init(NULL);
    mysql_real_connect(con,"localhost","fyl","123456","test_connect",0,NULL,CLIENT_MULTI_STATEMENTS);
    while(1){
        memset(buffer,0,sizeof(buffer));
        int len = recv(conn, buffer, sizeof(buffer),0);

        //没收到消息直接下一次循环
        if(len==0)
            continue;

        string str(buffer);
        //cout<<"用户"<<inet_ntoa(clnt_adr.sin_addr)<<"正在连接";

        //登录
        if(str.find("login")!=str.npos){
            int p1=str.find("login"),p2=str.find("pass:");
            name=str.substr(p1+5,p2-5);
            pass=str.substr(p2+5,str.length()-p2-4);
            string search="SELECT * FROM user WHERE NAME=\"";
            search+=name;
            search+="\";";
            cout<<endl<<"sql语句:"<<search<<endl;
            auto search_res=mysql_query(con,search.c_str());
            auto result=mysql_store_result(con);
            int col=mysql_num_fields(result);//获取列数
            int row=mysql_num_rows(result);//获取行数
            //auto info=mysql_fetch_row(result);//获取一行的信息
            if(search_res==0&&row!=0){
                cout<<"查询成功\n";
                //auto result=mysql_store_result(con);
                //int col=mysql_num_fields(result);//获取列数
                //int row=mysql_num_rows(result);//获取行数
                auto info=mysql_fetch_row(result);//获取一行的信息
                cout<<"查询到用户名:"<<info[0]<<" 密码:"<<info[1]<<endl;
                if(info[1]==pass){
                    cout<<"登录密码正确\n";
                    char str1[100]="ok";
                    if_login=true;
                    login_name=name;
                    name_sock_map[name]=conn;//记录下名字和文件描述符的对应关系
                    send(conn,str1,strlen(str1),0);
                }
                else{
                    cout<<"登录密码错误\n";
                    char str1[100]="wrong";
                    send(conn,str1,strlen(str1),0);
                }   
            }
            else{
                cout<<"查询失败\n";
                char str1[100]="wrong";
                send(conn,str1,strlen(str1),0);
            }
        }

        //注册
        else if(str.find("name:")!=str.npos){
            int p1=str.find("name:"),p2=str.find("pass:");
            name=str.substr(p1+5,p2-5);
            pass=str.substr(p2+5,str.length()-p2-4);
            string search="INSERT INTO user VALUES (\"";
            search+=name;
            search+="\",\"";
            search+=pass;
            search+="\");";
            cout<<endl<<"sql语句:"<<search<<endl;
            mysql_query(con,search.c_str());
        } 
        
        //绑定源用户和目的用户
        else if(str.find("target:")!=str.npos){
            int pos1=str.find("from");
            string target=str.substr(7,pos1-7),from=str.substr(pos1+4);
            //pair<string,int> tmp1(from,name_sock_map[from]);
            //pair<string,int> tmp2(target,name_sock_map[target]);
            //from_to_map[tmp1]=tmp2;
            target_conn=name_sock_map[target];
        }

        //接收到消息，转发
        else if(str.find("content:")!=str.npos){
            char recv_buff[1000];
            memset(recv_buff,0,sizeof(recv_buff));
            int len=recv(conn,recv_buff,sizeof(recv_buff),0);
            string recv_str(recv_buff);
            string send_str=recv_str.substr(7);
            send(target_conn,send_str.c_str(),send_str.length(),0);
        }

    }  
    mysql_close(con);
    close(conn);
}

