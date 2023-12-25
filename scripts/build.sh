#!/bin/bash

mkdir -p ./bin
mkdir -p ./bin/src
mkdir -p ./bin/tests

mkdir -p ./obj
mkdir -p ./obj/src
mkdir -p ./obj/tests

gcc ./build.c -Wall -Wextra -Wpedantic -Werror -Wunused-variable -std=c99 -O0 -g -o ./bin/build  
