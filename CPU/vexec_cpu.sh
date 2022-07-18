#!/bin/bash

make

valgrind --leak-check=full --track-origins=yes ./CPU.out
