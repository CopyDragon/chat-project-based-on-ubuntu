#!/bin/bash
echo "清理旧文件"
make clean
echo "\n开始编译"
make
echo "\n开始执行server"
./server
