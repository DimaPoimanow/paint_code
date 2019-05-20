#include "colors.h"
//This is header of function, which is used for paint with html
//Headers is needed to compile files, which use this functions (for main)

//To use html_paint: 1) User have to call function start_with_html (ONLY ONE TIME IN THE BEGINNING!)
//                   2) User have to call function paint_buf_html with parametrs - pointer (char *), amount of byte(which you want to print)
//                                                                                 and color (const int from colors.h)
//                   3) User call function end_with_html (ONLY ONE TIME IN THE END!)

//1:
int
start_with_html(void);

//2:
int
paint_buf_html(char **buf, unsigned long *size, int color);

//3:
int
end_with_html(void);
