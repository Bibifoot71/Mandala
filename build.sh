#!/bin/bash

gcc main.c `sdl2-config --libs --cflags` -lm -o Mandala
