/*************************************************************************
	> File Name: test.cpp
	> Author: 
	> Mail: 
	> Created Time: Sun Oct 18 16:54:37 2020
 ************************************************************************/

#include<iostream>
#include <mutex>
#include <atomic>
#include <string>
#include <memory>
#include <mysql/mysql.h>
#include <vector>
#include <iostream>
using namespace std;
int main(){
    MYSQL *con = mysql_init(NULL);
    mysql_real_connect(con, "127.0.0.1", "root", "",
                                                  "test_connect", 0, NULL, CLIENT_MULTI_STATEMENTS);
    string str="INSERT INTO user VALUES ('FYL','abc123');";
    if(!mysql_query(con,str.c_str()))
        cout<<"success!";
}

