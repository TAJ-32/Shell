#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "shell.h"

int main(int argc, char *argv)
{
	char *cmd;

	do
	{
		print_prompt1();
		cmd = read_cmd();

		if (!cmd) {
			printf("No command input");
			exit(EXIT_SUCCESS);
		}
		if (cmd[0] == '\0' || strcmp(cmd, "\n") == 0)//if nothing is input as a command, free the allocated memory given to cmd so that we can allocate it based on the command that is actually input
		{
			free(cmd);
			continue;

		}
		if (strcmp(cmd, "exit\n") == 0) //if they input exit and then hit enter
		{
			free(cmd);
			break;
		}

		printf("%s\n", cmd);
		free(cmd); //free the memory we used to store the command because it has been printed now
	} while (1);

	exit(EXIT_SUCCESS);

}

char *read_cmd() { //will read user's command 1024 bytes at a time

	char buf[1024]; //the max chunk of a user command we can read at one time
	char* ptr = NULL; //setting our pointer to null because it shouldn't point to any mem address right now, we haven't decided where we will store the user's command
	char  ptrlen = 0;

	while (fgets(buf, 1024, stdin)) //while we have something from the standard input to read (at most 1024 bytes)
	{
		int buflen = strlen(buf); //pinpointing exactly how much data we need to allocate for our pointer based on the length of the input
		if (!ptr) {
			ptr = malloc(buflen + 1); //allocating that memory for our pointer
		}
		else {
			char* ptr2 = realloc(ptr, ptrlen + buflen + 1); //if a pointer already exists, we are adding the memory already allocated to the pointer from our previous iteration to the memory we need allocated from this new chunk of command we are looking at
			if (ptr2) { //if the new memory we allocate in our realloc is anything
				ptr = ptr2; //set pointer to this new pointer that has enough memory for both the previous chunks of command the loop has read and the new one it is currently reading.
			}
			else {
				free(ptr); //if there is nothing to reallocate, we will free the space we gave to pointer
				ptr = NULL; //and reset pointer to point at nothing
			}
		}
		if (!ptr) {
			fprintf(stderr, "error: failed to allocate buffer: %s\n", strerror(errno));
			return NULL;
		}

		strcpy(ptr + ptrlen, buf);//copying each character byte into each one byte memory block we allocated for pointer
		if (buf[buflen - 1] == '\n') { //if last character in command is new line
			if (buflen == 1 || buf[buflen - 2] != '\\') { //if what we are looking at is either one character or a line end character before the new line
				return ptr;
			}
			ptr[ptrlen+buflen-2] = '\0';//the pointer should not include the newline char or the line end char
			buflen -=2; //get rid of the newline and endline chars in the space we are allocating
			print_prompt2();
		}
		ptrlen += buflen; //update the pointerlength to match how long the input is at this point
	}
	return ptr;
}
