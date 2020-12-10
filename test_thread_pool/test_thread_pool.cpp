/*************************************************************************
	> File Name: test_thread_pool.cpp
	> Author: 
	> Mail: 
	> Created Time: Thu Dec 10 16:58:09 2020
 ************************************************************************/

#include <iostream>
#include <thread>
#include <boost/bind.hpp>  
#include <boost/asio.hpp>
using namespace std;

void my_task(int a)
{
    cout<<a<<endl;
}

//此cpp文件用于测试boost库线程池的使用

int main(){
    /* 定义一个4线程的线程池 */
        boost::asio::thread_pool tp(4);

    /* 将函数投放到线程池 */
    for( int i=0; i<5; ++i  ) 
        boost::asio::post(boost::bind(my_task,i)); // 带参数的执行函数 

    /* 退出所有线程 */
    tp.join();

    return 0;
}

