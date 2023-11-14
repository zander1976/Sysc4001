#!/bin/sh
gcc main.c -DGUI -o maingui

./maingui test_memory_1.csv FCFS 4 500,250,150,100
