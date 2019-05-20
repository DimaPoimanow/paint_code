#include <stdio.h>
         //This is headers of function, which is used for find and determine first token from file
         //Headers are needed to compile files, which uses this functions (for main )
         //Also it consist of type of struct, which is used for find_token, and consts
         //for find_token and paint buffer
#ifndef TOKEN
#define TOKEN
    typedef struct Token
    {
        char *buf;
        unsigned long len;
        int type_of_token;
        int end_state;
    }Token;
#endif

//To use this modul: A)You have to create struct Token and initialize it.
//                   B)You have to call function find_token:
//                     Parametrs: 1)File, which contains symbols to analyze
//                                2)Pointer to struct Token
//                   C)Analyze the result of this modul: buf = pointer to memory with token
//                                                       len = size of memory
//                                                       type_of_token = token from list (const int)
//                                                       end_state => if end_state == NO_END, it means, that token is full and file is not end
//                                                                    if end_state == END it means, that file is end, and token can be not full

//State of reading:
enum {NOT_END = 0};
enum {END = 1};

//Type of tokens:
#ifndef TOKENS
#define TOKENS
enum {NO_TOKEN = 7};
enum {KEY_WORD = 0};
enum {IDENTIFIER = 1};
enum {FLOAT_CONST_DECIMAL = 2};
enum {STRING_LITERAL = 3};
enum {PUNCTUATOR = 4};
enum {COMMENT = 5};
enum {FLOAT_CONST_HEXADECIMAL = 6};
#endif

int
initialization_token(struct Token *Token);

int
find_token(int file, struct Token *Token);










