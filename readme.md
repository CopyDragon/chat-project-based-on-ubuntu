---
项目作者：fuyinglong

邮箱：838106527@qq.com

CSDN 昵称：你喜欢梅西吗

github 主页：https://github.com/CopyDragon

---

## 项目名：chat-project-based-on-ubuntu

### 介绍

使用 C++实现的 ubuntu 环境下的聊天小项目，采用 C/S 架构，支持注册、登录、记录登录状态、私聊、群聊功能，前期使用多线程实现并发服务器，后期利用 epoll 监听+boost 库线程池处理的 Reactor 模式实现并发服务器，进行了压力测试，并采用 bitmap 实现的布隆过滤器减少对 MySQL 的查询。

项目中使用 TCP 网络编程实现 C/S 的信息交互，使用 Mysql 记录用户账号、密码，使用 redis 记录用户的登录状态，编写了 makefile 进行编译，使用 shell 脚本提高了开发效率，开发过程使用 git 进行版本管理，编写了说明文档。

### 主要功能：

1、用户注册，数据存储到服务器主机的数据库中

2、用户登录

3、私聊

4、群聊

5、记录用户登录状态，五分钟内重启进程都不需要重新登陆

### 项目环境：


1、ubuntu 20.04.1

2、vi 编辑器

3、g++

4、Mysql 5.7.31

5、redis 4.0.8

6、boost 库 1.71 版本

7、hiredis 库

### 主要技术：

1、C++语言、STL 库容器和函数

2、多线程实现并发服务器

3、IO 多路复用+线程池实现并发服务器（使用 epoll 的 ET 边缘触发、EPOLLONESHOT）

4、使用 boost 库的线程池实现并发服务器

5、TCP socket 网络编程

6、Mysql 数据库以及 SQL 语句

7、redis 数据库（HASH 数据类型、设置键的过期时间）

8、session、cookie（利用 redis 保存 session 对象，服务器随机生成 sessionid 发往客户端保存到 cookie)

9、线程互斥锁

10、makefile 编译

11、git 版本管理

12、shell 脚本测试

### 设计思路：

1、用 Mysql 记录客户的账号和密码，注册和登录都要经过 Mysql

2、使用 C/S 模型完成私聊和群聊功能，所有的请求和聊天记录都会经过服务器并转发，减轻客户端压力，客户端只维护和服务器的 TCP 连接

3、利用 Redis 记录用户登录状态（HASH 类型，键为 sessionid，值为 session 对象，键五分钟后过期），当用户成功登录时服务器会利用随机算法生成 sessionid 发送到客户端保存，客户登录时会优先发送 cookie 到服务器检查，如果检查通过就不用输入账号密码登录。

4、循序渐进实现了三种服务器：多线程服务器、线程池服务器、IO 多路复用+线程池服务器

5、IO 多路复用+线程池服务器：采用 epoll 的边缘触发 ET 模式，对所有的读事件感兴趣，监听到某个 socket 上有事件触发时将通知线程池处理，线程池中的工作线程从缓冲区中读出数据并进行业务处理，同时 epoll 采用 EPOLLSHOT 模式，防止多个线程在同一 socket 上处理。

### Redis 记录登录状态：

    假设用户xiaoming登录，服务器随机生成的sessionid为1a2b3c4DEF，
    那么会执行如下的redis插入语句：hset 1a2b3c4DEF name xiaoming ，
    然后执行如下语句设置过期时间为300秒：expire 1a2b3c4DEF 300 ，
    服务器将该sessionid发往客户端作为cookie保存，客户端在重新启动进程会将cookie发往服务器，
    服务器收到客户端发来的sessionid后查询，使用如下语句：hget 1a2b3c4DEF name，
    只要该sessionid还未过期，就可以查询到结果，告知客户端登陆成功以及用户名。
    （注：redis查看所有键可用keys *语句）

### 生成 sessionid 的随机算法：

sessionid 大小为 10 位，每位由数字、小写字母、大写字母随机组成，理论上有(9+26+26)^10 种组合

### 压力测试思路

client.cpp 会向服务器发起若干个并发连接，并在这些连接上对服务器发出登录请求，每次使用的用户名和密码都是从本地的 account.txt 文件中抽取，具体发动多少轮请求可以自己设置

### bitmap 实现的布隆过滤器优化：

服务器启动时，会先根据所有数据来初始化布隆过滤器，当收到登录请求时，会先根据布隆过滤器来判断该用户名是否一定不存在，如果能够判断不存在就不会查询 MySQL 数据库，减小开销。

### 文件说明：

1、log.txt：git 导出的版本日志，记录了版本更新历史

2、test_mysql 文件夹：里面的文件用于测试和数据库是否成功建立连接，和项目没有直接联系

3、start_mysql.sh:启动、登录数据库的 shell 脚本，使用命令 sh start_mysql.sh 执行

4、makefile：用于编译

5、make_save:makefile 的副本

6、global.h、global.cpp：声明全局变量

7、server.cpp：服务器的基础代码

8、HandleServer.h、HandleServer.cpp：服务器的线程函数代码

9、client.cpp：客户端的基础代码

10、HandleClient.h、HandleClient.cpp：客户端的线程函数代码

11、make_and_run.sh：编译运行的 shell 脚本

12、cookie.txt：把程序跑起来才会在客户端产生的文件，保存 sessionid

13、test_thread_pool 文件夹：里面的文件用于测试 boost 库的线程池使用

14、start_redis.sh：快速启动 redis 服务的 shell 脚本，使用命令 source start_redis.sh 执行

15、serverUseThreadPool.cpp：利用线程池实现的服务器的基本代码

16、HandleServerUseThreadPool.cpp：线程池实现的服务器调用的线程函数代码

17、server：可执行文件，多线程服务器

18、serverUseThreadPool：可执行文件，线程池服务器

19、client：可执行文件，客户端

20、serverV2.cpp：IO 多路复用+线程池实现的并发服务器 2.0

21、HandleServerV2.cpp：serverV2 使用线程池调用该函数处理事件

22、HandleServerV2.h：文件 21 的头文件

23、serverV2：可执行文件，IO 多路复用+线程池实现的服务器

24、stress_test 文件夹：里面的文件用于压力测试，具体的说明可见文件夹中的 aboutStressTest.md 文件

25、asio 文件夹：包含 boost 库的 asio 库源代码，可以看该源代码来了解线程池实现

### 特别注意：

server、serverUseThreadPool、serverV2 都是服务器，只运行其中一个即可，server 是普通的多线程服务器，serverUseThreadPool 是用线程池实现的服务器，serverV2 是 IO 多路复用+线程池实现的服务器

### 运行环境说明：

1、基于 ubuntu 系统

2、装有 g++编译器及相关组件

3、服务器安装了 mysql（安装教程：https://blog.csdn.net/weixin_44164489/article/details/108926885）

4、服务器安装了 redis（安装教程：https://blog.csdn.net/weixin_44164489/article/details/109015099)

5、安装了 boost 库 1.71 版本

6、安装了 hiredis 库（安装教程：https://blog.csdn.net/weixin_44164489/article/details/110876479)

### 使用说明：

1、首先在 mysql 控制台创建一个数据库叫 test_connect，再创一个表叫 user，表有两项 VARCHAR 类型属性：NAME 和 PASSWORD，将 NAME 设为主键

2、然后修改 server 和 serverUseThreadPool.cpp 代码中的 ip 地址，更改为自己的服务器 ip 地址

3、启动 Mysql、redis 服务

4、执行 make_and_run 脚本得到可执行文件 client、server、serverUseThreadPool、serverV2

5、用一个终端先运行 server 或者 serverUseThreadPool 或者 serverV2

6、再开另外一个或多个终端运行 client

### 备注

1、数据库的名字和表的名字可以通过修改代码来自由决定

2、服务器 ip 地址可自行修改
