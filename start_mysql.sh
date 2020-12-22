#!/bin/bash
echo "开启mysql服务"
sudo service mysql start
echo "\n登录mysql"
mysql -u root #如果有密码就末尾加多个-p，mysql -u root -p

