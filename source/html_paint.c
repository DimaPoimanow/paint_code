#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include "html_paint.h"

//This is realization of paint_modul. It needs color and buffer. Function paint symbols in needed color.
//This modul will be execute if command has --html teg.

static int
prepare_html_buf(char **buf,unsigned long *size){
    for (unsigned long i = 0; i < (*size); i++) {
        if ((*buf)[i] == '&') {
            *buf = realloc(*buf, (*size) + 4); //&amp
            for (unsigned long j = (*size) + 3; j > i + 3; j--){
                (*buf)[j] = (*buf)[j-3];
            }
            (*buf)[i+1] = 'a';
            (*buf)[i+2] = 'm';
            (*buf)[i+3] = 'p';
            (*size) = (*size) + 3;
            i = i + 3;
            continue;
        }
        if ((*buf)[i] == '<') {
            *buf = realloc(*buf, (*size) + 3); //&lt
            for (unsigned long j = (*size) + 2; j > i + 2; j--){
                (*buf)[j] = (*buf)[j-2];
            }
            (*buf)[i] = '&';
            (*buf)[i+1] = 'l';
            (*buf)[i+2] = 't';
            (*size) = (*size) + 2;
            i = i + 2;
            continue;
        }
        if ((*buf)[i] == '>') {
            *buf = realloc(*buf, (*size) + 3); //&gt
            for (unsigned long j = (*size) + 2; j > i + 2; j--){
                (*buf)[j] = (*buf)[j-2];
            }
            (*buf)[i] = '&';
            (*buf)[i+1] = 'g';
            (*buf)[i+2] = 't';
            (*size) = (*size) + 2;
            i = i + 2;
            continue;
        }
        if ((*buf)[i] == '"') {
            *buf = realloc(*buf, (*size) + 5); //&quot
            for (unsigned long j = (*size) + 4; j > i + 4; j--){
                (*buf)[j] = (*buf)[j-4];
            }
            (*buf)[i] = '&';
            (*buf)[i+1] = 'q';
            (*buf)[i+2] = 'u';
            (*buf)[i+3] = 'o';
            (*buf)[i+4] = 't';
            (*size) = (*size) + 4;
            i = i + 4;
            continue;
        }
    }
    return 0;
}

int
paint_buf_html(char **buf, unsigned long *size, int color) {
    char *start_line = 0;
    char *end_line = 0;
    unsigned long add_size = 0;
    prepare_html_buf(buf, size);
    switch (color) {
    case YELLOW:
        start_line = "<font color=\"Yellow\">";
        end_line = "</font>";
        break;
    case GREEN:
        start_line = "<font color=\"Lime\">";
        end_line = "</font>";
        break;
    case RED:
        start_line = "<font color=\"Crimson\">";
        end_line = "</font>";
        break;
    case PINK:
        start_line = "<font color=\"MediumVioletRed\">";
        end_line = "</font>";
        break;
    case ORANGE:
        start_line = "<font color=\"DarkOrange\">";
        end_line = "</font>";
        break;
    case BLUE:
        start_line = "<font color=\"RoyalBlue\">";
        end_line = "</font>";
        break;
    case NO_COLOR:
        return 0;
    }
    char *c = malloc((*size) + 1);
    memcpy(c,*buf,(*size) + 1);
    add_size = strlen(start_line) + strlen(end_line);
    *size = (*size) + add_size;
    *buf = realloc(*buf, (*size) * sizeof(char) + 1);
    memcpy(*buf, start_line, strlen(start_line) + 1);
    strcat(*buf, c);
    strcat(*buf, end_line);
    free(c);
    return 0;
}

int
start_with_html() {
    write(1, "<html>\n<body>\n<listing>\n", strlen("<html>\n<body>\n<listing>\n"));
    return 0;
}

int
end_with_html() {
   write(1,"</listing>\n</body>\n</html>", strlen("</listing>\n</body>\n</html>"));
    return 0;
}
