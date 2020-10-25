/*************************************************************************
	> File Name: handle_server.cpp
	> Author: fuyinglong
	> Mail: 838106527@qq.com
	> Created Time: Tue Oct 20 16:11:22 2020
 ************************************************************************/

#include"HandleServer.h"
#include"global.h"

//extern unordered_map<pair<int,string>,pair<int,string>> from_to_map;//记录目标用户、源用户
extern unordered_map<string,int> name_sock_map;//名字和套接字描述符
extern unordered_map<int,set<int>> group_map;//记录群号和套接字描述符集合

void* handle_all_request(void *arg){
    pthread_mutex_t mutx;//互斥锁，锁住需要修改name_sock_map的临界区
    pthread_mutex_t group_mutx;//互斥锁，锁住修改group_map的临界区
    pthread_mutex_init(&mutx, NULL); //创建互斥锁
    pthread_mutex_init(&group_mutx,NULL);//创建互斥锁
    int conn=*(int *)arg;
    int target_conn=-1;
    char buffer[1000];
    string name,pass;
    bool if_login=false;//记录当前服务对象是否成功登录
    string login_name;//记录当前服务对象的名字
    string target_name;//记录发送信息时目标用户的名字
    int group_num;//记录群号
    MYSQL *con=mysql_init(NULL);
    mysql_real_connect(con,"localhost","fyl","123456","test_connect",0,NULL,CLIENT_MULTI_STATEMENTS);
    while(1){
        cout<<"-----------------------------\n";
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
            cout<<"sql语句:"<<search<<endl;
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
                    pthread_mutex_lock(&mutx); //上锁
                    name_sock_map[name]=conn;//记录下名字和文件描述符的对应关系
                    pthread_mutex_unlock(&mutx); //解锁
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
        
        //设定目标的文件描述符
        else if(str.find("target:")!=str.npos){
            int pos1=str.find("from");
            string target=str.substr(7,pos1-7),from=str.substr(pos1+4);
            //pair<string,int> tmp1(from,name_sock_map[from]);
            //pair<string,int> tmp2(target,name_sock_map[target]);
            //from_to_map[tmp1]=tmp2;
            target_name=target;
            if(name_sock_map.find(target)==name_sock_map.end())
                cout<<"源用户为"<<login_name<<",目标用户"<<target_name<<"仍未登陆，无法发起私聊\n";
            else{
                cout<<"源用户"<<login_name<<"向目标用户"<<target_name<<"发起的私聊即将建立";
                cout<<",目标用户的套接字描述符为"<<name_sock_map[target]<<endl;
                target_conn=name_sock_map[target];
            }       
        }

        //接收到消息，转发
        else if(str.find("content:")!=str.npos){
            if(target_conn==-1){
                cout<<"找不到目标用户"<<target_name<<"的套接字，将尝试重新寻找目标用户的套接字\n";
                if(name_sock_map.find(target_name)!=name_sock_map.end()){
                    target_conn=name_sock_map[target_name];
                    cout<<"重新查找目标用户套接字成功\n";
                }
                else{
                    cout<<"查找仍然失败，转发失败！\n";
                    continue;
                }
            }
            //char recv_buff[1000];
            //memset(recv_buff,0,sizeof(recv_buff));
            //int len=recv(conn,recv_buff,sizeof(recv_buff),0);
            string recv_str(str);
            string send_str=recv_str.substr(8);
            cout<<"用户"<<login_name<<"向"<<target_name<<"发送:"<<send_str<<endl;
            send_str="["+login_name+"]:"+send_str;
            send(target_conn,send_str.c_str(),send_str.length(),0);
        }
        
        //绑定群聊号
        else if(str.find("group:")!=str.npos){
            string recv_str(str);
            string num_str=recv_str.substr(6);
            group_num=stoi(num_str);
            cout<<"用户"<<login_name<<"绑定群聊号为："<<num_str<<endl;
            pthread_mutex_lock(&group_mutx);//上锁
            //group_map[group_num].push_back(conn);
            group_map[group_num].insert(conn);
            pthread_mutex_unlock(&group_mutx);//解锁
        }

        //广播群聊信息
        else if(str.find("gr_message:")!=str.npos){
            string send_str(str);
            send_str=send_str.substr(11);
            send_str="["+login_name+"]:"+send_str;
            cout<<"群聊信息："<<send_str<<endl;
            for(auto i:group_map[group_num]){
                if(i!=conn)
                    send(i,send_str.c_str(),send_str.length(),0);
            }       
        }
    }  
    mysql_close(con);
    close(conn);
}

