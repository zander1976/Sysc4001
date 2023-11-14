#!/bin/sh
gcc main.c -DGUI -o maingui

./maingui test_scheduler_1.csv RR 0
./maingui test_scheduler_2.csv RR 0
