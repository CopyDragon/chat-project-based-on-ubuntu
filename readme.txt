--------------------------------------------------
                                    
   项目作者：fuyinglong
   邮箱：838106527@qq.com
   CSDN昵称：你喜欢梅西吗
   github主页：https://github.com/CopyDragon

--------------------------------------------------

chat-project-based-on-ubuntu
使用C++实现的ubuntu环境下的聊天小项目，支持注册、登录、私聊、群聊功能，使用到的技术包括C++、TCP网络编程、多线程、Mysql、互斥锁等。

主要功能：
1、用户注册，数据存储到服务器主机的数据库中 
2、用户登录
3、私聊
4、群聊


项目环境：
ubuntu + vi编辑器


主要技术：
1、C++语言、STL库容器和函数
2、多线程实现并发服务器
3、TCP socket网络编程
4、Mysql数据库以及SQL语句
5、线程互斥锁
6、makefile编译
7、git版本管理
8、shell脚本测试


文件说明：
1、log.txt：git导出的版本日志，记录了版本更新历史
2、test_mysql文件夹：里面的文件用于测试和数据库是否成功建立连接，和项目没有直接联系
3、start_mysql.sh:启动、登录数据库的shell脚本
4、makefile：用于编译
5、make_save:makefile的副本
6、global.h、global.cpp：声明全局变量
7、server.cpp：服务器的基础代码
8、HandleServer.h、HandleServer.cpp：服务器的线程函数代码
9、client.cpp：客户端的基础代码
10、HandleClient.h、HandleClient.cpp：客户端的线程函数代码
11、make_and_run.sh：编译运行的shell脚本

运行环境说明：
1、基于ubuntu系统
2、装有g++编译器及相关组件
3、安装了mysql（安装教程：https://blog.csdn.net/weixin_44164489/article/details/108926885）

使用说明：
1、首先在mysql控制台创建一个数据库叫test_connect，再创一个表叫user
2、然后修改代码中的ip地址，更改为自己的服务器ip地址
2、执行make_and_run脚本得到可执行文件client、server
3、用一个终端先运行server
4、再开另外一个或多个终端运行client

备注
1、数据库的名字和表的名字可以通过修改代码来自由决定
2、服务器ip地址可自行修改


