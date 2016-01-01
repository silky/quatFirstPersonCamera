#!/bin/bash
cd "$(dirname -- "$(readlink -fn -- "$0")")"

cc main.c -lSDL2 -lGLEW -lGL -lm -o app
