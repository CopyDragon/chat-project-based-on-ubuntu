## 关于stress_test文件夹及压力测试的说明

### 文件说明
ServerV2.cpp等与服务器相关的源文件：就是服务器代码，没有特别修改过
client.cpp：客户端代码，用来进行压力测试，与普通的客户端不同
AddAccount.cpp：用来随机生成若干个账户信息的代码，会将生成的账户信息写入到MySQL以及保存到本地txt文件
addaccount：AddAccount.cpp编译得到的可执行文件
account.txt：执行addaccount会将账户信息写入到该txt
result.txt：记录压力测试的一些结果

### 压力测试思路
client.cpp会向服务器发起若干个并发连接，并在这些连接上不断对服务器发出登录请求，每次使用的用户名和密码都是随机从本地的account.txt文件中抽取，客户端每隔1秒钟发动一波攻势。


