#!/bin/bash

make

valgrind --leak-check=full ./Memoria.out
