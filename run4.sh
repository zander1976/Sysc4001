#!/bin/sh
gcc main.c -o main

echo "Unlimited"
./main test_memory_1.csv FCFS 4 0,0,0,0

echo "50,50,50,50"
./main test_memory_1.csv FCFS 4 50,50,50,50

echo "500,50,50,50"
./main test_memory_1.csv FCFS 4 500,50,50,50

echo "500,500,50,50"
./main test_memory_1.csv FCFS 4 500,500,50,50

echo "300,300,350,50"
./main test_memory_1.csv FCFS 4 300,300,350,50
