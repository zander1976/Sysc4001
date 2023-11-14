#!/bin/sh
gcc main.c -o main

./main test_scheduler_1.csv RR 0
./main test_scheduler_2.csv RR 0
