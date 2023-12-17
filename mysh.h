#ifndef __MY_IO_H
#define __MY_IO_H

#include <stdbool.h>
int execute_program(char *curr_program, int other_side, int pipefd[], bool piping);

#endif
