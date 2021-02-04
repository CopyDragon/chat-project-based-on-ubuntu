/*************************************************************************
	> File Name: serverV2.cpp
	> Author: 
	> Mail: 
	> Created Time: Mon Dec 21 12:27:10 2020
 ************************************************************************/

#include "global.h"
#include "HandleServerV2.h"
using namespace std;

//#define MAXLINE 1000  
//#define OPEN_MAX 100

//listen的backlog大小
#define LISTENQ 200
//监听端口号
#define SERV_PORT 8000
#define INFTIM 1000 

//extern void handle_all_request(string epoll_str,int conn_num,int epollfd);
extern void* handle_all_request(void *arg);
extern unordered_map<string,int> name_sock_map;//记录名字和套接字描述符
//extern clock_t begin_clock;//开始时间，用于性能测试，有bug
extern double total_time;//线程池处理任务的总时间
//extern time_point<system_clock> begin_clock;//开始时间，压力测试
extern int total_handle;//总处理请求数，用于性能测试
extern int total_recv_request;//接收到的请求总数，性能测试
extern int Bloom_Filter_bitmap[1000000];//布隆过滤器所用的bitmap
extern queue<int> mission_queue;//任务队列
extern int mission_num;//任务队列中的任务数量
extern pthread_cond_t mission_cond;//线程池所需的条件变量
extern pthread_spinlock_t name_mutex;//互斥锁，锁住需要修改name_sock_map的临界区
extern pthread_spinlock_t from_mutex;//互斥锁，锁住修改from_to_map的临界区
extern pthread_spinlock_t group_mutex;//互斥锁，锁住修改group_map的临界区
extern pthread_mutex_t queue_mutex;//互斥锁，锁住修改任务队列的临界区
extern int epollfd;
extern pthread_spinlock_t count_mutex;

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
    pthread_spin_init(&name_mutex, 0); //创建互斥锁
    pthread_spin_init(&group_mutex,0);//创建互斥锁
    pthread_mutex_init(&queue_mutex,0);
    pthread_spin_init(&count_mutex,0);
    pthread_spin_init(&from_mutex,0);
    pthread_cond_init(&mission_cond,NULL);//初始化条件变量

    int i, maxi, listenfd, connfd, sockfd,epfd,nfds;  
    ssize_t n;  
    //char line[MAXLINE];  
    socklen_t clilen;  
    //声明epoll_event结构体的变量,ev用于注册事件,数组用于回传要处理的事件  
    struct epoll_event ev,events[10000];  
    //生成用于处理accept的epoll专用的文件描述符  
    epfd=epoll_create(10000);
    epollfd=epfd;
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
    serveraddr.sin_addr.s_addr = inet_addr("192.168.3.202");//此处设为服务器的ip
    serveraddr.sin_port=htons(8023);  
    bind(listenfd,(sockaddr *)&serveraddr, sizeof(serveraddr));  
    listen(listenfd, LISTENQ); 
    clilen=sizeof(clientaddr);
    maxi = 0;   
    
    cout<<"准备连数据库\n";

    //连接MYSQL数据库
    MYSQL *con=mysql_init(NULL);
    mysql_real_connect(con,"127.0.0.1","root","","test_connect",0,NULL,CLIENT_MULTI_STATEMENTS);
    string search="SELECT * FROM user;";
    auto search_res=mysql_query(con,search.c_str());
    auto result=mysql_store_result(con);
    int row;
    if(result)
        row=mysql_num_rows(result);//获取行数

    cout<<"连接数据库成功\n准备初始化布隆过滤器\n";

    //读取数据并完成布隆过滤器初始化
    memset(Bloom_Filter_bitmap,0,sizeof(Bloom_Filter_bitmap));
    for(int i=0;i<row;i++){
        auto info=mysql_fetch_row(result);//获取一行的信息
        string read_name=info[0];
        //对字符串使用哈希函数
        int hash=0;
        //cout<<"字符串："<<read_name;
        for(auto ch:read_name){
            hash=hash*131+ch;
            if(hash>=10000000)
                hash%=10000000;
        }
        hash%=32000000;
        //cout<<",hash值为："<<hash;
        //调整bitmap
        int index=hash/32,pos=hash%32;
        //cout<<index<<" "<<pos<<endl;
        Bloom_Filter_bitmap[index]|=(1<<pos);
        //cout<<",调整后的："<<Bloom_Filter_bitmap[index]<<endl;
    }
    mysql_close(con);
    int one=0,zero=0;
    for(auto i:Bloom_Filter_bitmap){
        int b=1;
        for(int j=1;j<=32;j++){
            if(b&i)
                one++;
            else
                zero++;
            b=(b<<1);
        }
    }
    cout<<"布隆过滤器中共有"<<one<<"位被置为1，其余"<<zero<<"位仍为0"<<endl;

    /* 定义一个10线程的线程池 */
    //boost::asio::thread_pool tp(10);

    //10个线程的线程池
    pthread_t tid[10];
    for(int i=0;i<10;i++)
        pthread_create(&tid[i],NULL,handle_all_request,NULL);

    //压力测试
    total_time=0;
    total_handle=0;
    total_recv_request=0;

    while(1){  
        cout<<"--------------------------"<<endl;
        cout<<"epoll_wait阻塞中"<<endl;
        //等待epoll事件的发生  
        nfds=epoll_wait(epfd,events,10000,-1);//最后一个参数是timeout，0:立即返回，-1:一直阻塞直到有事件，x:等待x毫秒
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
                total_recv_request++;
                sockfd = events[i].data.fd;
                events[i].data.fd=-1;
                cout<<"接收到读事件"<<endl;
                
                pthread_mutex_lock(&queue_mutex);
                mission_queue.push(sockfd);//加入任务队列
                pthread_cond_broadcast(&mission_cond);//广播唤醒
                pthread_mutex_unlock(&queue_mutex);

                //string recv_str;
                //boost::asio::post(boost::bind(handle_all_request,recv_str,sockfd,epfd)); //加入任务队列，处理事件
            }  
        } 
    }  
    close(listenfd);
}
