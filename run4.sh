#!/bin/sh
gcc main.c -o main

./main test_scheduler_1.csv FCFS 4 300,300,350,50
