#!/bin/bash
mkdir ./bin
g++ -march=noavx -g -Wall parser.cpp -o parser