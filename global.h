/*************************************************************************
	> File Name: global.h
	> Author: fuyinglong
	> Mail: 838106527@qq.com
	> Created Time: Wed Oct 21 16:41:24 2020
 ************************************************************************/

#ifndef _GLOBAL_H
#define _GLOBAL_H

#include<unordered_map>
#include<vector>
#include<set>
using namespace std;

extern unordered_map<string,int> name_sock_map;//记录名字和文件描述符
extern unordered_map<int,set<int>> group_map;//记录群号和对应的文件描述符集合

#endif
