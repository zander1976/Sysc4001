#!/bin/sh
gcc main.c -o main

./main test_scheduler_1.csv FCFS 0
./main test_scheduler_2.csv FCFS 0
