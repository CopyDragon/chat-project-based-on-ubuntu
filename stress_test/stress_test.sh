#!/bin/bash
for((i=0; i<20; i++));do
    ./client &
done
sleep 100
pkill client

