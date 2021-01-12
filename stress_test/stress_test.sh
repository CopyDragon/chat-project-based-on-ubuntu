#!/bin/bash
for((i=0; i<2; i++));do
    ./client &
done
sleep 10
echo "执行 pkill client"
pkill client

