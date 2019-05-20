#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include "find_token.h"
#include "bash_paint.h"
#include "html_paint.h"
#include "out_print.h"

static int
determine_color_to_token(int type_of_token){
    switch (type_of_token){
    case KEY_WORD:
        return YELLOW;
    case IDENTIFIER:
        return ORANGE;
    case STRING_LITERAL:
        return BLUE;
    case COMMENT:
        return GREEN;
    case FLOAT_CONST_DECIMAL:
        return RED;
    case FLOAT_CONST_HEXADECIMAL:
        return RED;
    case PUNCTUATOR:
        return PINK;
    case NO_TOKEN:
        return NO_COLOR;
    }
    return -1;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr,"No name of file\n");
        return -1;
    }
    struct Token Token;
    initialization_token(&Token);
    int file = open(argv[1], O_RDONLY);
    int variant_of_paint = 0;
    int (*paint_buf)(char **, unsigned long *, int ) = 0;                  //pointer to paint function
    if (file == -1) {
        perror("FILE IS NOT OPEN\n");                                      //check it for openning
        return -1;
    }
    if (argc < 3){
        fprintf(stderr,"No way to paint\n");
        return -1;
    }
    if (!strncmp(argv[2],"-html", strlen(argv[2]))){                       //Check for paint teg
        start_with_html();
        paint_buf = paint_buf_html;
        variant_of_paint = 0;
    } else if (!strncmp(argv[2],"-bash", strlen(argv[2]))){
        paint_buf = paint_buf_bash;
        variant_of_paint = 1;
    } else {
        fprintf(stderr,"No way to paint\n");
        return -1;
    }
    while (Token.end_state != END) {
        find_token(file, &Token);                                           //read buffer and determine the token
        int color = determine_color_to_token(Token.type_of_token);          //define color
        paint_buf(&Token.buf, &Token.len, color);                           //paint text (token)
        out_print(Token.buf, Token.len);                                    //print text (token)
        free(Token.buf);                                                    //free memory
    }

    if (variant_of_paint == 0){
        end_with_html();
    }
   close(file);                                                         	//close file
}
