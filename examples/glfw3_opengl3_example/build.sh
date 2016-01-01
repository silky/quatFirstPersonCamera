#!/bin/bash
cd "$(dirname -- "$(readlink -fn -- "$0")")"

cc main.c `pkg-config --static --libs glfw3` -lGLEW -lGL -lm -o app
