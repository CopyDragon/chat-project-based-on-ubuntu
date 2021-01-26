## 关于 stress_test 文件夹及压力测试的说明

### 文件说明

ServerV2.cpp 等与服务器相关的源文件：就是服务器代码，没有特别修改过

client.cpp：客户端代码，用来进行压力测试，与普通的客户端不同

AddAccount.cpp：用来随机生成若干个账户信息的代码，会将生成的账户信息写入到 MySQL 以及保存到本地 txt 文件

addaccount：AddAccount.cpp 编译得到的可执行文件

account.txt：执行 addaccount 会将账户信息写入到该 txt

result.txt：记录压力测试的一些结果

### 压力测试思路

client.cpp 会向服务器发起若干个并发连接，并在这些连接上对服务器发出登录请求，每次使用的用户名和密码都是从本地的 account.txt 文件中抽取，具体发动多少轮登陆请求可以自己设置。
