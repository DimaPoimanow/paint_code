#include <stdio.h>
#include <string.h>
#include "find_token.h"
#include "bash_paint.h"

//This is realization of paint_modul. It needs color and buffer. Function paint symbols in needed color.
//This modul will be execute if command has --bush teg.

int
paint_buf_bash(char *buf, int *state) {
    switch (*state) {
    case NO_TOKEN:
        if (strlen(buf) == 0){
            fwrite(buf,sizeof(char),1,stdout);
        } else {
            fprintf(stdout, "%s", buf);                             //change color and write buffer in this color
        }
        break;                                                      //in the end change color back for avoid troubles
    case KEY_WORD:
        fprintf(stdout, "\033[01;33m%s\033[00m", buf);
        break;
    case IDENTIFIER:
        fprintf(stdout, "\033[38;05;208m%s\033[00m", buf);
        break;
    case FLOAT_CONST_DECIMAL:
        fprintf(stdout, "\033[01;31m%s\033[00m", buf);
        break;
    case FLOAT_CONST_HEXADECIMAL:
        fprintf(stdout, "\033[01;31m%s\033[00m", buf);
        break;
    case STRING_LITERAL:
        fprintf(stdout, "\033[01;34m%s\033[00m", buf);
        break;
    case COMMENT:
        fprintf(stdout, "\033[01;32m%s\033[00m", buf);
        break;
    case PUNCTUATOR:
        fprintf(stdout, "\033[38;05;200m%s\033[00m", buf);
        break;
  }
    return 0;
}
