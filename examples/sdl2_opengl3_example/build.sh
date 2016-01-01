#!/bin/bash
cd "$(dirname -- "$(readlink -fn -- "$0")")"

cc -std=c99 main.c -lSDL2 -lGLEW -lGL -lm -o app
