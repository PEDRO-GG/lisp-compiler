#!/bin/bash

mkdir -p ./bin
mkdir -p ./obj
gcc ./build.c -Wall -Wextra -Wpedantic -Werror -Wunused-variable -std=c99 -O0 -g -o ./bin/build  
