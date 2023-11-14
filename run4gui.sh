#!/bin/sh
gcc main.c -DGUI -o maingui

echo "Unlimited"
./maingui test_memory_1.csv FCFS 4 0,0,0,0

echo "300,300,350,50"
./main test_memory_1.csv FCFS 4 300,300,350,50
