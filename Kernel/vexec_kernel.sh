#!/bin/bash

make

valgrind -v ./Kernel.out
