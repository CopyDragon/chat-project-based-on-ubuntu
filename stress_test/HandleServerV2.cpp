/*************************************************************************
	> File Name: HandleServerV2.cpp
	> Author: 
	> Mail: 
	> Created Time: Tue Dec 22 14:01:16 2020
 ************************************************************************/

#include"HandleServerV2.h"
#include"global.h"

extern unordered_map<string,int> name_sock_map;//名字和套接字描述符
extern unordered_map<int,set<int>> group_map;//记录群号和套接字描述符集合
extern unordered_map<string,string> from_to_map;//记录用户xx要向用户yy发送信息
//extern clock_t begin_clock;//开始时间，用于性能测试
//extern time_point<system_clock> begin_clock;
extern int total_handle;//总处理请求数，用于性能测试
extern int total_recv_request;//接收到的请求总数，性能测试
extern double top_speed;//峰值性能
extern int Bloom_Filter_bitmap[1000000];//布隆过滤器所用的bitmap
extern queue<int> mission_queue;//任务队列
extern int mission_num;//任务队列中的任务数量
extern pthread_cond_t mission_cond;//线程池所需的条件变量
extern pthread_spinlock_t name_mutex;//互斥锁，锁住需要修改name_sock_map的临界区
extern pthread_spinlock_t group_mutex;//互斥锁，锁住修改group_map的临界区
extern pthread_spinlock_t from_mutex;//互斥锁，锁住修改from_to_map的临界区
extern pthread_mutex_t queue_mutex;//互斥锁，锁住修改任务队列的临界区
extern int epollfd;
extern pthread_spinlock_t count_mutex;

void* handle_all_request(void *arg){   //(string epoll_str,int conn_num,int epollfd){
    while(1){
        int conn_num;
        //取出任务
        pthread_mutex_lock(&queue_mutex);
        while(mission_queue.empty()){
            pthread_cond_wait(&mission_cond,&queue_mutex);
        }
        conn_num=mission_queue.front();
        mission_queue.pop();
        pthread_mutex_unlock(&queue_mutex);

        time_point<system_clock> begin_clock= system_clock::now();
        //pthread_spin_init(&mutex, NULL); //创建互斥锁
        //pthread_spin_init(&group_mutex,NULL);//创建互斥锁
        int conn=conn_num;
        int target_conn=-1;
        char buffer[1000];
        string name,pass;
        bool if_login=false;//记录当前服务对象是否成功登录
        string login_name;//记录当前服务对象的名字
        string target_name;//记录发送信息时目标用户的名字
        int group_num;//记录群号
        
        
        //连接MYSQL数据库
        MYSQL *con=mysql_init(NULL);
        mysql_real_connect(con,"127.0.0.1","root","","test_connect",0,NULL,CLIENT_MULTI_STATEMENTS);
        
        //连接redis数据库(2020.12.9)
        redisContext *redis_target = redisConnect("127.0.0.1",6379);
        if(redis_target->err){
            redisFree(redis_target);
            cout<<"连接redis失败"<<endl;     
        }
        
        //禁用nagle算法，防止粘包
        //int enable = 1;
        //setsockopt(conn, IPPROTO_TCP, TCP_NODELAY, (void*)&enable, sizeof(enable));

        cout<<"-----------------------------\n";
        string recv_str;
        while(1){
            char buf[10];
            memset(buf, 0, sizeof(buf));
            int ret  = recv(conn, buf, sizeof(buf), 0);
            if(ret < 0){
                cout<<"recv返回值小于0"<<endl;
                //对于非阻塞IO，下面的事件成立标识数据已经全部读取完毕
                if((errno == EAGAIN) || (errno == EWOULDBLOCK)){
                      printf("数据读取完毕\n");
                    cout<<"接收到的完整内容为："<<recv_str<<endl;
                    cout<<"开始处理事件"<<endl;
                    break;
                }
                cout<<"errno:"<<errno<<endl;
                close(conn);
                mysql_close(con);
                if(!redis_target->err)
                    redisFree(redis_target);
                //events[i].data.fd=-1;
                return NULL;
            }
            else if(ret == 0){
                cout<<"recv返回值为0"<<endl;
                close(conn);
                mysql_close(con);
                if(!redis_target->err)
                    redisFree(redis_target);
                return NULL;
                //events[i].data.fd=-1;
            }
            else{
                printf("接收到内容如下: %s\n",buf);
                string tmp(buf);
                recv_str+=tmp;
            }
        }
        string str=recv_str;

        //2020.12.9新增：先接收cookie看看redis是否保存该用户的登录状态
        if(str.find("cookie:")!=str.npos){
            string cookie=str.substr(7);
            string redis_str="hget "+cookie+" name";
            redisReply *r = (redisReply*)redisCommand(redis_target,redis_str.c_str());
            string send_res;
            if(r->str){
                cout<<"查询redis结果："<<r->str<<endl;
                send_res=r->str;
                pthread_spin_lock(&name_mutex); //上锁
                name_sock_map[send_res]=conn;//记录下名字和文件描述符的对应关系
                pthread_spin_unlock(&name_mutex); //解锁
                //cout<<sizeof(r->str)<<endl;
                // cout<<send_res.length()<<endl;
            }
            else
                send_res="NULL";
            send(conn,send_res.c_str(),send_res.length()+1,0);
            //if(r->str==)
        }

        //登录
        else if(str.find("login")!=str.npos){
            int p1=str.find("login"),p2=str.find("pass:"),flag=0;
            name=str.substr(p1+5,p2-5);
            pass=str.substr(p2+5,str.length()-p2-4);

            //2021.1.25：新增布隆过滤器
            //对字符串使用哈希函数
            int hash=0;
            for(auto ch:name){
                hash=hash*131+ch;
                if(hash>=10000000)
                    hash%=10000000;                            
            }
            int index=hash/32,pos=hash%32;
            if((Bloom_Filter_bitmap[index]&(1<<pos))==0){
                cout<<"布隆过滤器查询为0，登录用户名必然不存在数据库中\n";
                char str1[100]="wrong";
                send(conn,str1,strlen(str1),0);
                flag=1;
            }
            
            //布隆过滤器无法判断才要查数据库
            if(flag==0){
                string search="SELECT * FROM user WHERE NAME=\"";
                search+=name;
                search+="\";";
                cout<<"sql语句:"<<search<<endl;
                auto search_res=mysql_query(con,search.c_str());
                auto result=mysql_store_result(con);
                //int col=mysql_num_fields(result);//获取列数
                int row;
                if(result)
                    row=mysql_num_rows(result);//获取行数
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
                        string str1="ok";
                        if_login=true;
                        login_name=name;
                        pthread_spin_lock(&name_mutex); //上锁
                        name_sock_map[name]=conn;//记录下名字和文件描述符的对应关系
                        pthread_spin_unlock(&name_mutex); //解锁

                        //2020.12.9新添加：随机生成sessionid并发送到客户端
                        srand(time(NULL));//初始化随机数种子
                        for(int i=0;i<10;i++){
                            int type=rand()%3;//type为0代表数字，为1代表小写字母，为2代表大写字母
                            if(type==0)
                                str1+='0'+rand()%9;
                            else if(type==1)
                                str1+='a'+rand()%26;
                            else if(type==2)
                                str1+='A'+rand()%26;
                        }
                        //将sessionid存入redis
                        string redis_str="hset "+str1.substr(2)+" name "+login_name;
                        redisReply *r = (redisReply*)redisCommand(redis_target,redis_str.c_str());
                        //设置生存时间,默认300秒
                        redis_str="expire "+str1.substr(2)+" 300";
                        r=(redisReply*)redisCommand(redis_target,redis_str.c_str());

                        cout<<"随机生成的sessionid为："<<str1.substr(2)<<endl;
                        //cout<<"redis指令:"<<r->str<<endl;

                        send(conn,str1.c_str(),str1.length()+1,0);
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
            string target=str.substr(7,pos1-7),from=str.substr(pos1+5);
            //pair<string,int> tmp1(from,name_sock_map[from]);
            //pair<string,int> tmp2(target,name_sock_map[target]);
            //from_to_map[tmp1]=tmp2;
            target_name=target;
            if(name_sock_map.find(target)==name_sock_map.end())
                cout<<"源用户为"<<from<<",目标用户"<<target_name<<"仍未登陆，无法发起私聊\n";
            else{
                pthread_spin_lock(&from_mutex);
                from_to_map[from]=target;
                pthread_spin_unlock(&from_mutex);
                login_name=from;
                cout<<"源用户"<<login_name<<"向目标用户"<<target_name<<"发起的私聊即将建立";
                cout<<",目标用户的套接字描述符为"<<name_sock_map[target]<<endl;
                target_conn=name_sock_map[target];
            }       
        }

        //接收到消息，转发
        else if(str.find("content:")!=str.npos){
            //根据两个map找出当前用户和目标用户
            for(auto i:name_sock_map){
                if(i.second==conn){
                    login_name=i.first;
                    target_name=from_to_map[i.first];
                    target_conn=name_sock_map[target_name];
                    break;
                }       
            }
            if(target_conn==-1){
                cout<<"找不到目标用户"<<target_name<<"的套接字，将尝试重新寻找目标用户的套接字\n";
                if(name_sock_map.find(target_name)!=name_sock_map.end()){
                    target_conn=name_sock_map[target_name];
                    cout<<"重新查找目标用户套接字成功\n";
                }
                else{
                    cout<<"查找仍然失败，转发失败！\n";
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
            //找出当前用户
            for(auto i:name_sock_map)
                if(i.second==conn){
                    login_name=i.first;
                    break;
                }
            cout<<"用户"<<login_name<<"绑定群聊号为："<<num_str<<endl;
            pthread_spin_lock(&group_mutex);//上锁
            //group_map[group_num].push_back(conn);
            group_map[group_num].insert(conn);
            pthread_spin_unlock(&group_mutex);//解锁
        }

        //广播群聊信息
        else if(str.find("gr_message:")!=str.npos){
            //找出当前用户
            for(auto i:name_sock_map)
                if(i.second==conn){
                    login_name=i.first;
                    break;
                }
            //找出群号
            for(auto i:group_map)
                if(i.second.find(conn)!=i.second.end()){
                    group_num=i.first;
                    break;
                }
            string send_str(str);
            send_str=send_str.substr(11);
            send_str="["+login_name+"]:"+send_str;
            cout<<"群聊信息："<<send_str<<endl;
            for(auto i:group_map[group_num]){
                if(i!=conn)
                    send(i,send_str.c_str(),send_str.length(),0);
            }       
        }  
        cout<<"---------------------"<<endl;

        //2021.1.6添加：线程工作完毕后重新注册事件
        epoll_event event;
        event.data.fd=conn;
        event.events=EPOLLIN|EPOLLET|EPOLLONESHOT;
        epoll_ctl(epollfd,EPOLL_CTL_MOD,conn,&event);
        
        mysql_close(con);
        if(!redis_target->err)
            redisFree(redis_target);

        //2021.1.11:性能测试
        auto end_clock   = system_clock::now();
        auto duration = duration_cast<microseconds>(end_clock - begin_clock);
        pthread_spin_lock(&count_mutex);
        total_time+=double(duration.count()) * microseconds::period::num / microseconds::period::den; 
        total_handle++;
        pthread_spin_unlock(&count_mutex);
        //double total_time=(double)(end_clock-begin_clock)/CLOCKS_PER_SEC;
        //cout<<begin_clock<<" "<<end_clock<<endl;
        double now_rate=total_handle/total_time;
        if(now_rate>top_speed)
            top_speed=now_rate;
        cout<<"已用时"<<total_time<<"秒,";
        cout<<"共收到"<<total_recv_request<<"个请求,";
        cout<<"已处理"<<total_handle<<"个请求\n";
        cout<<"处理一个请求平均需要"<<total_time/total_handle<<"秒,";
        cout<<"平均一秒处理"<<now_rate<<"个请求\n";
        cout<<"峰值性能为一秒处理"<<top_speed<<"个请求";
        cout<<"---------------------------------\n";
    }   
}
