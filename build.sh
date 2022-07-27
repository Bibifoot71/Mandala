#!/bin/bash

gcc -W -Werror -Wall  -Wextra -O3 main.c `sdl2-config --libs --cflags` -lm -o Mandala
