#!/bin/bash

make

valgrind --leak-check=full ./Consola.out ./cfg/proceso1.txt 1
