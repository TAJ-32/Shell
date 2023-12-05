#ifndef __MY_IO_H
#define __MY_IO_H

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>
#include <stdbool.h>
#include <string.h>
#include <fcntl.h>

char **parse_line(char *args[]); 

#endif
