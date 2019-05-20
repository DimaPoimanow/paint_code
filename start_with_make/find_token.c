#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include "buferization.h"
#include "find_token.h"

//This realization of modul which is used to find token from beginning of the file.
//The main function find_token return value of token in the text and the buffer with symbols from token.
//This modul is a kind of Analizator of the text.

static STATE_BUF buf_state[NUM_TOKEN];                                       
static unsigned long counter = 0;
static int count_digit_in_a_row = 0;

static int
symbol_is_digit(char c) {
    if((c >= '0') && (c <= '9')) {                                  //This function is to determinate: symbol c is digit or no-digit
        return 1;                                                   //yes - return 1, no - return 0
    }
    return 0;
}

static int
symbol_is_non_digit(char c) {
    if ((c >= 'A') && (c <= 'Z')) {
        return 1;                           						//This function helps to determine:is char c non-digit from standart?
    } else if ((c >= 'a') && (c <= 'z')) {   						//yes - return 1, no - return 0
        return 1;
    } else if (c == '_') {
        return 1;
    }
    return 0;
}

static int
buf_state_default(void) {
    for (int i = 0; i < NUM_TOKEN; i++) {
        buf_state[i].buf_state = 1;         						//Function sets buffer's states in default: buf_state[i] = 1
        buf_state[i].inside_state = 0;                              //(all tokens are possible). inside_state = 0 (beginning)
    }
    return 0;
}

static int
symbol_is_simple_escape(char c) {
    static const char simple_escape[SIMPLE_ESC] = {'\'', '\"', '?', '\\','a','b','f','n','r','t','v','\n'};
    for (int i = 0; i < SIMPLE_ESC; i++) {
        if (c == simple_escape[i]) {  							//This function helps to determine:is char c simple-escape-charecter from standart?
            return 1;                   						//yes - return 1, no - return 0
        }
    }
    return 0;
}

static int
symbol_is_octal_escape(char digit) {
    if ((digit >= '0') && (digit <= '7')) {
        return 1;                   							//This function helps to determine:is char c octal-escape-character from standart?
    }                                   						//yes - return 1, no - return 0
    return 0;
}

static int
symbol_is_hexadecimal(char c) {
    if ((c >= '0') && (c <= '9')) {
        return 1;
    } else if ((c >= 'a') && (c <= 'f')) {  						//This function helps to determine:is char c
        return 1;                           						// hexadecimal digit from standart? yes - return 1, no - return 0
    } else if ((c >= 'A') && (c <= 'F')) {
        return 1;
    }
    return 0;
}

static int
symbol_is_float_suffix(char c) {
    if ((c == 'f') || (c == 'l') || (c == 'F') || (c == 'L')) {
        return 1;                                                       //This function helps to determine:is char c
    }                                                 					// float-suffix from standart? yes - return 1, no - return 0
    return 0;
}

static int
find_token_buf_state(void) {
    for (int i = 0; i < NUM_TOKEN; i++){
        if (buf_state[i].buf_state == 1) {
            return i;
        }                   								//function is called, then only one token is possible
    }                       								//helps to know: which token is possible now?
                                                                                         //returns number of token
    return 0;
}

static int
buf_state_NO(int token) {
    buf_state[token].buf_state = 0;    							//sets the state of token to NO-state
    buf_state[token].inside_state = 0;  						//it means that this token is not possible in this buffer
    return 0;
}

static int
symbol_is_begin_of_punctuator(char c){
    const char punctuator[14] = {'/', '%', '>', '<', '^', '|', '=', '!',
                                '&', '*', '+', '-', '^', ':'};
    for (int i = 0; i < 14; i++){
        if (c == punctuator[i]){
            return 1;
        }
    }
    return 0;

}

int
check_for_punctuator_from_list(char *buf){
    const char *punctuator[NUM_OF_PUNC] = {"/", "%", ">", "<", "^", "|", "=", "<<", ">>", "<=", ">=", "==", "!=",
                                           "&&", "||", "*=", "/=", "%=", "+=", "-=", "&=", "^=", "|=", "<:", ":>",
                                           "<%", "%>", "%:", ">>=", "<<=", "%:%:"};
    unsigned long len = strlen(buf);
    for (int i = 0; i < NUM_OF_PUNC; i++){
        if (strlen(punctuator[i]) ==len) {
            if (!strncmp(buf,punctuator[i], len)){
                return 1;
            }
        }
    }
    return 0;
}

int
find_count(char *buf){
    int len = strlen(buf);
    int count = 0;
    for (int i = len-1;;i--){
        if (symbol_is_hexadecimal(buf[i])){
            count++;
            continue;
        } else {
            return count+1;
        }
    }
}

static int
check_for_key_word(char *buf) {                                     //it is a multi-state machine. it check the buffer on key word
    const char *key_word[NUM_OF_KEY] = {"if", "inline", "int", "long", "register", "restrict",
                                        "return", "short", "signed", "sizeof", "static", "struct",
                                        "switch", "typedef", "union"};
    if (buf_state[KEY_WORD].buf_state == 0) {
        return 0;                                 					//if key word is impossible in this buffer -> return 0
    }
    unsigned long len = strlen(buf);                                //length of this buffer
    if ((len+1) == counter){
        len = counter;
    }
    int count_of_simple = 0;
    int count_similar = 0;
    switch (buf_state[KEY_WORD].inside_state) {    					//this machine check the last reading symbol and current state -->
    case 0:                                                                 //change state and do something
        for (int i = 0; i < NUM_OF_KEY; i++){
            if (strncmp(buf, key_word[i],len) == 0) {
                count_of_simple++;
                if (len == strlen(key_word[i])) {
                    count_similar++;
                }
            }                                                      			//first state wait for similar buffer with one of key_words
        }                                                           			//if it exists, change inside state to 1
        if (count_of_simple == 0) {
            buf_state_NO(KEY_WORD);
            return 0;
        }
        if ((count_of_simple == 1) && (count_similar == 1)) {
            buf_state[KEY_WORD].buf_state = 1;
            buf_state[KEY_WORD].inside_state = 1;
            return 0;
        }
        if ((count_of_simple >= 1) && (count_similar == 0)) {
        buf_state[KEY_WORD].buf_state = 1;
        buf_state[KEY_WORD].inside_state = 0;
        return 0;
        }
        break;
    case 1:
        if ((symbol_is_non_digit(buf[len-1])) || (symbol_is_digit(buf[len-1]))) {
             buf_state_NO(KEY_WORD);
        } else {
            buf_state[KEY_WORD].inside_state = 2;
        }
        break;
    }
    return 0;
}

static int
check_for_identifier(char *buf) {                  					//it is a multi-state machine. check buffer for identifier
    if (buf_state[IDENTIFIER].buf_state == 0) {     				// my specification - white space must be after identifier
        buf_state_NO(IDENTIFIER);                  				//if identifier is impossible in this buffer - return 0
        return 0;
    }
    unsigned long len = strlen(buf);
    if ((len+1) == counter){
        len = counter;
    }
    switch (buf_state[IDENTIFIER].inside_state) {  				//this machine check the last reading symbol and current state
    case 0:                                                           //hange state and do something
        if (symbol_is_non_digit(buf[0])) {
            buf_state[IDENTIFIER].inside_state = 1;
        } else if (buf[0] == '\\') {                            		//in 0: check for first symbol^ it must be non-digit
            buf_state[IDENTIFIER].inside_state = 2;                 		//true - change for state 1, else it is not identifier
        } else {
            buf_state_NO(IDENTIFIER);
            return 0;
        }
        break;
    case 1:
        if ((symbol_is_digit(buf[len-1])) || (symbol_is_non_digit(buf[len-1]))) {
            buf_state[IDENTIFIER].inside_state = 1;
        } else if (buf[len-1] == '\\') {                            		//continue check for non-digit or digit
            buf_state[IDENTIFIER].inside_state = 2;                 		// or check for white space (
        } else if ((len > 1) && (buf_state[STRING_LITERAL].buf_state == 0))  {
            buf_state[IDENTIFIER].inside_state = 5;
        } else {
            buf_state_NO(IDENTIFIER);
            return 0;
        }
        break;
    case 2:
        if (buf[len-1] == 'u') {
            buf_state[IDENTIFIER].inside_state = 3;                         //separately I check for universal character names in identifier
        } else if (buf[len-1] == 'U') {                             		//after \u must be 4 digits, after \U - 8 digits
            buf_state[IDENTIFIER].inside_state = 4;
        } else {                                                        	//else - it is not identifier
            buf_state_NO(IDENTIFIER);
            return 0;
        }
        break;
    case 3:
        if (symbol_is_hexadecimal(buf[len-1])) {
            count_digit_in_a_row++;
            if (count_digit_in_a_row < 4) {
                break;
            } else {                                                    //check for 4 digits after \u
                buf_state[IDENTIFIER].inside_state = 1;
                count_digit_in_a_row = 0;
            }
        } else {
            buf_state[IDENTIFIER].inside_state = 6;
            count_digit_in_a_row = 0;
            return 0;
        }
        break;
    case 4:
        if (symbol_is_hexadecimal(buf[len-1])) {
            count_digit_in_a_row++;
            if (count_digit_in_a_row < 8) {
                break;                                      			//check for 8 digits after \U
            } else {
                buf_state[IDENTIFIER].inside_state = 1;
                count_digit_in_a_row = 0;
            }
        } else {
            buf_state[IDENTIFIER].inside_state = 6;
            count_digit_in_a_row = 0;
            return 0;
        }
        break;
    }
    return 0;
}

static int
check_for_hexadecimal_const(char *buf) {                                //it is a multi-state machine. check buffer for hexadecimal const.
    if (buf_state[FLOAT_CONST_HEXADECIMAL].buf_state == 0) {            //I devided float const into
        return 0;                                                       //16 and 10 systems, because they are really different
    }
    unsigned long len = strlen(buf);
    if ((len+1) == counter){
        len = counter;
    }
    switch (buf_state[FLOAT_CONST_HEXADECIMAL].inside_state) {
    case 0:
        if (buf[0] == '0') {                                        	//check for 0 in buffer
            buf_state[FLOAT_CONST_HEXADECIMAL].inside_state = 1;
        } else {
            buf_state_NO(FLOAT_CONST_HEXADECIMAL);
            return 0;
        }
        break;
    case 1:
        if ((buf[1] == 'x') || (buf[1] == 'X')) {
            buf_state[FLOAT_CONST_HEXADECIMAL].inside_state = 2;
        } else {                                                        	//check for x or X after 0 (standart rule)
            buf_state_NO(FLOAT_CONST_HEXADECIMAL);
            return 0;
        }
        break;
    case 2:
        if (symbol_is_hexadecimal(buf[len-1])) {
            buf_state[FLOAT_CONST_HEXADECIMAL].inside_state = 3;
        } else if (buf[len-1] == '.'){                                  	//check for digits or .
            buf_state[FLOAT_CONST_HEXADECIMAL].inside_state = 4;         	//wait for . after digits (must be >= 1 digit)
        } else {                                                            	//else - not a const
            buf_state_NO(FLOAT_CONST_HEXADECIMAL);
            return 0;
        }
        break;
    case 3:
        if (symbol_is_hexadecimal(buf[len-1])) {
            return 0;
        } else if (buf[len-1] == '.'){                                  	//continue check for digits
            buf_state[FLOAT_CONST_HEXADECIMAL].inside_state = 4;         	//wait for . after digits
        } else {                                                            	// check for exponent part after dots od digSits
            buf_state_NO(FLOAT_CONST_HEXADECIMAL);
            return 0;
        }
        break;
    case 4:
        if (symbol_is_hexadecimal(buf[len-1])) {
            return 0;
        } else if ((buf[len-1] == 'p') || (buf[len-1] == 'P')) {        	//check for exponent part
            buf_state[FLOAT_CONST_HEXADECIMAL].inside_state = 5;
        } else {
            if (len > 4) {
                buf_state[FLOAT_CONST_HEXADECIMAL].inside_state = 8;
            } else {
                buf_state_NO(FLOAT_CONST_HEXADECIMAL);
                return 0;
            }
        }
        break;
    case 5:
        if (symbol_is_hexadecimal(buf[len-1])) {
            buf_state[FLOAT_CONST_HEXADECIMAL].inside_state = 7;        	//check for sign
        } else if ((buf[len-1] == '+') || (buf[len-1] == '-')) {
            buf_state[FLOAT_CONST_HEXADECIMAL].inside_state = 6;
        } else {
            buf_state_NO(FLOAT_CONST_HEXADECIMAL);
            return 0;
        }
        break;
    case 6:
        if (symbol_is_digit(buf[len-1])){
            buf_state[FLOAT_CONST_HEXADECIMAL].inside_state = 7;            	//check for digits after sign or p or P (must be >= digit)
        } else {                                                                //else - not a const
            buf_state_NO(FLOAT_CONST_HEXADECIMAL);
            return 0;
        }
        break;
    case 7:
        if (symbol_is_digit(buf[len-1])) {
            return 0;
        } else if (symbol_is_float_suffix(buf[len-1])) {                    	//check for digits until the end or float suffix at the end
            return 0;
        } else {
            buf_state[FLOAT_CONST_HEXADECIMAL].inside_state = 8;
        }
        break;
    }
    return 0;
}

static int
check_for_decimal_const(char *buf){
    if (buf_state[FLOAT_CONST_DECIMAL].buf_state == 0) {  			//this function is similar with check_for_hexadecimal_const.
        return 0;                                           			//the main difference - digits are decimal and const haven't got suffix 0x at the beginning
    }
    unsigned long len = strlen(buf);
    if ((len+1) == counter){
        len = counter;
    }
    switch (buf_state[FLOAT_CONST_DECIMAL].inside_state) {   			//ALL STATES LIKE check_for_hexadecimal_const
    case 0:
        if (symbol_is_digit(buf[0])) {
            buf_state[FLOAT_CONST_DECIMAL].inside_state = 1;
        } else if (buf[0] == '.') {
            buf_state[FLOAT_CONST_DECIMAL].inside_state = 2;
        } else {
            buf_state_NO(FLOAT_CONST_DECIMAL);
            return 0;
        }
        break;
    case 1:
        if (symbol_is_digit(buf[len-1])) {
            return 0;
        } else if (buf[len-1] == '.') {
            buf_state[FLOAT_CONST_DECIMAL].inside_state = 2;
        } else {
            buf_state_NO(FLOAT_CONST_DECIMAL);
            return 0;
        }
        break;
    case 2:
        if (symbol_is_digit(buf[len-1])) {
            return 0;
        } else if ((buf[len-1] == 'e') || (buf[len-1] == 'E')) {
            buf_state[FLOAT_CONST_DECIMAL].inside_state = 3;
        } else {
            if (len > 2) {
                buf_state[FLOAT_CONST_DECIMAL].inside_state = 6;
            } else {
                buf_state_NO(FLOAT_CONST_DECIMAL);
                return 0;
            }
        }
        break;
    case 3:
        if (symbol_is_digit(buf[len-1])) {
            buf_state[FLOAT_CONST_DECIMAL].inside_state = 5;
        } else if ((buf[len-1] == '+') || (buf[len-1] == '-')) {
            buf_state[FLOAT_CONST_DECIMAL].inside_state = 4;
        } else {
            buf_state_NO(FLOAT_CONST_DECIMAL);
            return 0;
        }
        break;
    case 4:
        if (symbol_is_digit(buf[len-1])) {
            buf_state[FLOAT_CONST_DECIMAL].inside_state = 5;
        } else {
            buf_state_NO(FLOAT_CONST_DECIMAL);
            return 0;
        }
        break;
    case 5:
        if (symbol_is_digit(buf[len-1])) {
            return 0;
        } else if (symbol_is_float_suffix(buf[len-1])) {
            return 0;
        } else {
            buf_state[FLOAT_CONST_DECIMAL].inside_state = 6;
        }
        break;
    }
    return 0;
}

static int
check_for_string_literal(char *buf) {
    if (buf_state[STRING_LITERAL].buf_state == 0) {
        return 0;
    }                                                           		//this is multi-state machine to check buffer for string literals
    unsigned long len = strlen(buf);                                		//specification: Must be 2 symbols " to determine string literal
    if ((len+1) == counter){
        len = counter;
    }
    switch (buf_state[STRING_LITERAL].inside_state) {				//if check for the string suffix, then for '"', and after all other symbols check for '"'
    case 0:
        if ((buf[0] == 'U') || (buf[0] == 'L')) {
            buf_state[STRING_LITERAL].inside_state = 1;
        } else if (buf[0] == 'u') {
            buf_state[STRING_LITERAL].inside_state = 2;
        } else if (buf[0] == '\"') {
            buf_state[STRING_LITERAL].inside_state = 3;
        } else {
            buf_state_NO(STRING_LITERAL);
            return 0;
        }
        break;
    case 1:
        if (buf[len-1] == '\"') {
            buf_state[STRING_LITERAL].inside_state = 3;
        } else {
            buf_state[STRING_LITERAL].inside_state = 10;
            return 0;
        }
        break;
    case 2:
        if (buf[len-1] == '8') {
            buf_state[STRING_LITERAL].inside_state = 1;
        } else if (buf[len-1] == '\"') {
            buf_state[STRING_LITERAL].inside_state = 3;
        } else {
            buf_state_NO(STRING_LITERAL);
            return 0;
        }
        break;
    case 3:
        if (buf[len-1] == '\"') {
            buf_state[STRING_LITERAL].buf_state = 1;
            buf_state[STRING_LITERAL].inside_state = 5;
            return 0;
        } else if (buf[len-1] == '\\') {
            buf_state[STRING_LITERAL].inside_state = 4;
        } else if (buf[len-1] == '\n') {
            buf_state[STRING_LITERAL].inside_state = 10;
            return 0;
        } else if (buf[len-1] == '\0') {
            buf_state[STRING_LITERAL].inside_state = 10;
            return 0;
        } else {
            buf_state[STRING_LITERAL].inside_state = 3;
            return 0;
        }
        break;
    case 4:
        if ((symbol_is_simple_escape(buf[len-1])) || (symbol_is_octal_escape(buf[len-1]))) {
            buf_state[STRING_LITERAL].inside_state = 3;
        } else if (buf[len-1] == 'x') {
            buf_state[STRING_LITERAL].inside_state = 6;
        } else {
            buf_state[STRING_LITERAL].inside_state = 10;
            return 0;
        }
        break;
    case 6:
        if (symbol_is_hexadecimal(buf[len-1])) {
            buf_state[STRING_LITERAL].inside_state = 3;
        } else {
            buf_state[STRING_LITERAL].inside_state = 10;
            return 0;
        }
        break;
    }
    return 0;
}

static int
check_for_punctuator(char *buf) {   
    const char *punctuator[NUM_OF_PUNC] = {"/", "%", ">", "<", "^", "|", "=", "<<", ">>", "<=", ">=", "==", "!=",
                                           "&&", "||", "*=", "/=", "%=", "+=", "-=", "&=", "^=", "|=", "<:", ":>",
                                           "<%", "%>", "%:", ">>=", "<<=", "%:%:"};
    if (buf_state[PUNCTUATOR].buf_state == 0) {
        return 0;                                       			//this is multi-state machine to check buffer for punctuators
    }                                                   			//I wait for first symbol isn't a part of punctuator and check buffer for punctuators
    unsigned long len = strlen(buf);                                //all punctuators in array char *punctuators[]
    if ((len+1) == counter){
        len = counter;
    }
    int count_similar = 0;
    switch (buf_state[PUNCTUATOR].inside_state) {
    case 0:
        if (symbol_is_begin_of_punctuator(buf[0])) {
            buf_state[PUNCTUATOR].inside_state = 1;
        } else {
            buf_state_NO(PUNCTUATOR);
        }
        break;
    case 1:
        for (int i=0; i < NUM_OF_PUNC; i++) {
            if (strlen(punctuator[i]) >= len){
                if (!strncmp(buf,punctuator[i], len)){
                    count_similar++;
                }
            }
        }
        if (count_similar > 0) {
            buf_state[PUNCTUATOR].inside_state = 1;
        } else {
            for (int i = 0; i < NUM_OF_PUNC; i++){
                if (strlen(punctuator[i]) == (len - 1)) {
                    if (!strncmp(buf,punctuator[i], len - 1)) {
                        buf_state[PUNCTUATOR].inside_state = 2;
                        return 0;
                    }
                }
            }
            buf_state_NO(PUNCTUATOR);
            return 0;
        }
        break;
    case 2:
        buf_state_NO(PUNCTUATOR);
        break;
    }
    return 0;
}

static int
check_for_comment(char *buf) {
    if (buf_state[COMMENT].buf_state == 0) {            			//this is multi-state machine to check buffer for comments
         return 0;                                        			//specification --> must be (*/ after /*) and (/n after //)
                                                                    //It wait for /
    }                                                      			//Check for // or /*
    unsigned long len = strlen(buf);
    if ((len+1) == counter){
        len = counter;
    }                                                                   //Wait for /n or */
    switch (buf_state[COMMENT].inside_state) {              			//else - it is not a comment
    case 0:
        if ((buf[0] == '/') && (len == 1)) {
            buf_state[COMMENT].buf_state = 1;
            buf_state[COMMENT].inside_state = 1;
        } else {
            buf_state_NO(COMMENT);
            return 0;
        }
        break;
    case 1:
        if (buf[1] == '/') {
            buf_state[COMMENT].inside_state = 2;
        } else if (buf[1] == '*') {
            buf_state[COMMENT].inside_state = 3;
        } else {
            buf_state_NO(COMMENT);
            return 0;
        }
        break;
    case 2:
        if (buf[len-1] == '\n') {
            buf_state[COMMENT].buf_state = 1;
            buf_state[COMMENT].inside_state = 6;
            return 0;
        } else if (buf[len-1] == '\\') {
            buf_state[COMMENT].inside_state = 5;
        } else {
        return 0;
        }
        break;
    case 3:
        if (buf[len-1] == '*') {
            buf_state[COMMENT].inside_state = 4;
        }
        break;
    case 4:
        if ((buf[len-1] == '/') && (buf[len-2] == '*')) {
            buf_state[COMMENT].buf_state = 1;
            buf_state[COMMENT].inside_state = 6;
            return 0;
        }
        break;
    case 5:
        buf_state[COMMENT].inside_state = 2;
        break;
    }
    return 0;
}

static int
check_for_token(char *buf, int *to_do, int *type_of_token) {
    check_for_string_literal(buf);
    check_for_key_word(buf);                   					//this function is calls check -functions of all tokens and wait for the determined state of buffer
    check_for_identifier(buf);                  				//it change the type_of_token to draw buffer, to_do state to stop or continue reading
    check_for_decimal_const(buf);               				//the criterion - mark
    check_for_hexadecimal_const(buf);             				//if mark = 0 --> no token                  				
    check_for_punctuator(buf);
    check_for_comment(buf);
    int mark = 0;
    for (int i = 0; i < NUM_TOKEN; i++) {
        mark = mark + buf_state[i].buf_state;
    }

    if (mark == 0) {
        *type_of_token = NO_TOKEN;
        *to_do = STOP;
        buf_state_default();
        return 0;
    } else if (mark == 1) {
        int token = find_token_buf_state();
        if (token == PUNCTUATOR) {
            if (buf_state[PUNCTUATOR].inside_state == 2) {
                *type_of_token = token;
                buf_state_default();
                *to_do = STOP;
                return 0;
            } else {
                *type_of_token = token;
                *to_do = CONTINUE;
                return 0;
            }
        }
        if (token == COMMENT) {
            if (buf_state[COMMENT].inside_state == 6) {
                *type_of_token = token;
                buf_state_default();
                *to_do = STOP;
                return 0;
            } else {
                *type_of_token = token;
                *to_do = CONTINUE;
                return 0;
            }
        }
        if (token == STRING_LITERAL) {
            if (buf_state[STRING_LITERAL].inside_state == 5) {
                *type_of_token = token;
                buf_state_default();
                *to_do = STOP;
                return 0;
            } else if (buf_state[STRING_LITERAL].inside_state == 10){
                *to_do = STOP;
                buf_state_default();
                *type_of_token = token;
                buf_state[STRING_LITERAL].buf_state = 0;
                buf_state[STRING_LITERAL].inside_state = 10;
                return 0;
            } else {
                *type_of_token = token;
                *to_do = CONTINUE;
                return 0;
            }
        }
        if (token == IDENTIFIER) {
            if (buf_state[IDENTIFIER].inside_state == 5) {
                *type_of_token = token;
                buf_state_default();
                *to_do = STOP;
                return 0;
            } else if (buf_state[IDENTIFIER].inside_state == 6){
                *type_of_token = token;
                *to_do = STOP;
                return 0;
            } else {
                *type_of_token = token;
                *to_do = CONTINUE;
                return 0;
            }
        }
        if (token == FLOAT_CONST_DECIMAL){
            if (buf_state[FLOAT_CONST_DECIMAL].inside_state == 6) {
                *type_of_token = token;
                buf_state_default();
                *to_do = STOP;
                return 0;
            } else {
                *type_of_token = token;
                *to_do = CONTINUE;
                return 0;
            }

        }
        if (token == FLOAT_CONST_HEXADECIMAL){
            if (buf_state[FLOAT_CONST_HEXADECIMAL].inside_state == 8) {
                buf_state[PUNCTUATOR].inside_state = 2;
                *type_of_token = token;
                buf_state_default();
                *to_do = STOP;
                return 0;
            } else {
                *type_of_token = token;
                *to_do = CONTINUE;
                return 0;
            }

        }
    } else if (mark == 2){
        if ((buf_state[IDENTIFIER].buf_state == 1) && (buf_state[KEY_WORD].buf_state == 1)) {
            if (buf_state[KEY_WORD].inside_state == 2) {
                buf_state_NO(IDENTIFIER);
                *type_of_token = KEY_WORD;
                *to_do = STOP;
                return 0;
            }
        }
        *to_do = CONTINUE;
        return 0;
    } else {
        *to_do = CONTINUE;
        return 0;
    }
    return 0;
}

int
find_token(int file, char **buf, int *end_state, int *type_of_token) {
    unsigned long pos = 0;
    buf_inizialization(&(*buf));
    long num = 0;
    counter = 0;
    int to_do = 0;
    buf_state_default();
    while ((num=read(file, ((*buf)+pos), sizeof(char))) > 0) {
        counter++;
        check_for_token(*buf, &to_do, type_of_token);
        if (to_do == STOP) {
            if ((counter > 1) && (*type_of_token == NO_TOKEN)) {
                if (lseek(file, -1,SEEK_CUR) == -1) {
                    perror("LSEEK is not correct!");
                    return -1;
                }
                buf_realloc(&(*buf), -1);
            }
            if (*type_of_token == KEY_WORD) {
                if (buf_state[*type_of_token].inside_state == 2) {
                    if (lseek(file, -1,SEEK_CUR) == -1) {
                        perror("LSEEK is not correct!");
                        return -1;
                    }
                    buf_realloc(&(*buf), -1);
                }
            }
            if (*type_of_token == IDENTIFIER){
                if (buf_state[IDENTIFIER].inside_state == 6){
                    if (lseek(file, -1,SEEK_CUR) == -1) {
                        perror("LSEEK is not correct!");
                        return -1;
                    }
                    buf_realloc(&(*buf), -1);
                    int count = find_count(*buf);
                    if (lseek(file, (-1)*count,SEEK_CUR) == -1) {
                        perror("LSEEK is not correct!");
                        return -1;
                    }

                    buf_realloc(&(*buf), (-1)*count);
                    *type_of_token = NO_TOKEN;
                    buf_state_default();
                    return 0;
                }
            }
            if ((*type_of_token == FLOAT_CONST_DECIMAL) || (*type_of_token == FLOAT_CONST_HEXADECIMAL)
                    || (*type_of_token == PUNCTUATOR) || (*type_of_token == IDENTIFIER)) {
                if (lseek(file, -1,SEEK_CUR) == -1) {
                    perror("LSEEK is not correct!");
                    return -1;
                }
                buf_realloc(&(*buf), -1);
            }
            if ((*type_of_token == STRING_LITERAL) && (buf_state[STRING_LITERAL].inside_state == 10)){
                to_do = 0;
                if (lseek(file, (-1)*(counter),SEEK_CUR) == -1) {
                    perror("LSEEK is not correct!");
                    return -1;
                }
                free(*buf);
                (*buf) = 0;
                buf_inizialization(&(*buf));
                counter = 0;
                pos = 0;
                num = 0;
                continue;
            }
            buf_state_default();
            return 0;
        }
        pos = strlen(*buf);
        buf_realloc(&(*buf), 1);
    }
    if ((num == 0) && (strlen(*buf) == 1) && ((*buf)[0] == ' ')){
		buf_realloc(&(*buf), -1);
	}	
    if (to_do != STOP){
        if (buf_state[KEY_WORD].inside_state == 1){
            *type_of_token =KEY_WORD;
            *end_state = END;
            return 0;
        }
        if (buf_state[IDENTIFIER].inside_state == 1){
            *type_of_token = IDENTIFIER;
            *end_state = END;
            return 0;
        }
        if (buf_state[STRING_LITERAL].inside_state == 3){
            *type_of_token = STRING_LITERAL;
            *end_state = END;
            return 0;
        }
        if (buf_state[PUNCTUATOR].inside_state == 1) {
            if (check_for_punctuator_from_list(*buf)){
                *type_of_token = PUNCTUATOR;
                *end_state = END;
                return 0;
            }
        }
        if ((buf_state[COMMENT].inside_state == 6) || (buf_state[COMMENT].inside_state == 2) ||
                (buf_state[COMMENT].inside_state == 3) || (buf_state[COMMENT].inside_state == 4)) {
            *type_of_token = COMMENT;
            *end_state = END;
            return 0;
        }
        if ((buf_state[FLOAT_CONST_DECIMAL].inside_state == 2) || (buf_state[FLOAT_CONST_DECIMAL].inside_state == 5)) {
            *type_of_token = FLOAT_CONST_DECIMAL;
            *end_state = END;
            return 0;
        }
        if ((buf_state[FLOAT_CONST_HEXADECIMAL].inside_state == 4) || (buf_state[FLOAT_CONST_HEXADECIMAL].inside_state == 7)) {
            *type_of_token = FLOAT_CONST_HEXADECIMAL;
            *end_state = END;
            return 0;
        }
        *end_state = END_WITHOUT_TOKEN;
    } else {
        *end_state = END;
    }
    return 0;
}
