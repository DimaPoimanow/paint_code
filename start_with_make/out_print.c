#include "out_print.h"
#include <unistd.h>

int
out_print(char *buf, unsigned long size){
    write(1, buf, size);
    return 0;
}
