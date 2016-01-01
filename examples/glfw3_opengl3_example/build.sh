#!/bin/bash
cd "$(dirname -- "$(readlink -fn -- "$0")")"

cc -std=c99 main.c `pkg-config --static --libs glfw3` -lGLEW -lGL -lm -o app
