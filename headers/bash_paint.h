#include "colors.h"
//This is header of function, which is used for paint with bash

//Header is needed to compile files, which uses this function (for main)

//It needs color, pointer to memory and size of memory. Function paint symbols in needed color (paste special symbols of colors in memory).
//To use this function you have to give pointer (char *), amout of byte (which you want to print)
//                                                              and color (const int from color.h).

int
paint_buf_bash(char **buf, unsigned long *size, int color);


