/*************************************************************************
	> File Name: server.cpp
	> Author: fuyinglong
	> Mail: 838106527@qq.com
	> Created Time: Sun Oct 18 16:06:56 2020
 ************************************************************************/

#include "HandleServerUseThreadPool.h"
#include "global.h"
using namespace std;

extern void handle_all_request(int);

//unordered_map<pair<int,string>,pair<int,string>> from_to_map;//记录源用户、目的用户
extern unordered_map<string,int> name_sock_map;//记录名字和套接字描述符

void tmptest(int a){
    cout<<a<<endl;    
}

int main(){
    int serv_sock, clnt_sock;
    sockaddr_in serv_adr,clnt_adr;
    socklen_t clnt_adr_sz;
    pthread_mutex_t mutx;

    pthread_mutex_init(&mutx, NULL); //创建互斥锁
    serv_sock = socket(PF_INET, SOCK_STREAM, 0);//PF_INET:tcp/ip协议   SOCK_STREAM: tcp    0:默认协议
    memset(&serv_adr, 0, sizeof(serv_adr));
    serv_adr.sin_family = AF_INET;//使用地址族
    serv_adr.sin_addr.s_addr = inet_addr("172.29.18.134");//ip地址
    serv_adr.sin_port = htons(8023);//端口号

    if (bind(serv_sock, (struct sockaddr *)&serv_adr, sizeof(serv_adr)) == -1)
         cout<<"bind() error";
    if (listen(serv_sock, 5) == -1)//连接队列大小为5
         cout<<"listen() error";

    //vector<int> sock_arr;//记录套接字描述符
    clnt_adr_sz = sizeof(clnt_adr);
    int conn;
    pthread_t t_id;//线程id

    //2020.12.10添加：新增boost库线程池
    /* 定义一个5线程的线程池 */
    boost::asio::thread_pool tp(5);

    while(conn = accept(serv_sock, (struct sockaddr *)&clnt_adr, &clnt_adr_sz)){
        //HandleServer target;
        //HandleServer::clnt_adr=clnt_adr;
        cout<<"用户"<<inet_ntoa(clnt_adr.sin_addr)<<"正在连接:\n";
        //cout<<conn<<endl;
        //sock_arr.push_back((conn);
        //2020.12.10添加：新增boost库线程池
        boost::asio::post(boost::bind(handle_all_request,conn)); // 执行函数

        //pthread_create(&t_id, NULL, handle_all_request, (void *)&conn); 
        //pthread_detach(t_id);  
    }
    tp.join();//释放线程池

    /*
    MYSQL *con=mysql_init(NULL);
    mysql_real_connect(con,"localhost","fyl","123456","test_connect",0,NULL,CLIENT_MULTI_STATEMENTS);

    char buffer[1000];
    string name,pass;
    while(1){
        memset(buffer,0,sizeof(buffer));
        int len = recv(conn, buffer, sizeof(buffer),0);

        //没收到消息直接下一次循环
        if(len==0)
            continue;

        string str(buffer);
        cout<<"用户"<<inet_ntoa(clnt_adr.sin_addr)<<"正在连接";

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
            if(search_res==0){
                cout<<"查询成功\n";
                auto result=mysql_store_result(con);
                int col=mysql_num_fields(result);//获取列数
                int row=mysql_num_rows(result);//获取行数
                auto info=mysql_fetch_row(result);//获取一行的信息
                cout<<"查询到用户名:"<<info[0]<<" 密码:"<<info[1]<<endl;
                if(info[1]==pass){
                    cout<<"登录密码正确\n";
                    char str1[100]="ok";
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
    } 
    mysql_close(con);*/
}
