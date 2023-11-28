mysh: mysh.c
	gcc -Wall -pedantic -g -o mysh mysh.c

.PHONE: clean
clean:
	rm -f mysh
