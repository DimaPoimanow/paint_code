#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "buferization.h"

//This realization of modul which is used to work with buffer. 1) First function to inizialization buffer,
//                              2) Second function to change buffer (to decrease or increase by several bytes)

int
buf_inizialization(char **buf){
    char *control_pointer;
    control_pointer = realloc(0, sizeof(char)*2);
    if (control_pointer == NULL) {                                      
        perror("Error");                                                
        free(*buf);                                                     
        return -1;                                                      
    }
    *buf = control_pointer;
    (*buf)[0] = ' ';
    (*buf)[1] = '\0';
    return 0;
}

int
buf_realloc(char **buf, int index){
    char *control_pointer;
    unsigned long len = strlen(*buf);
    control_pointer = realloc(*buf, len + 1 + index);
    if (control_pointer == NULL) {
        perror("Error");
        free(*buf);
        return -1;
    }
    *buf = control_pointer;
    (*buf)[len + index] = '\0';
    return 0;
}
