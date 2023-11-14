#!/bin/sh
gcc main.c -DGUI -o maingui

./maingui test_scheduler_1.csv FCFS 0
./maingui test_scheduler_2.csv FCFS 0
