#include "mysh.h"
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pwd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>

int main(int argc, char *argv[]) {

	char current_dir[256];
	
	if (getcwd(current_dir, sizeof(current_dir)) == NULL) {
		perror("getcwd() error");
	}
	else {
		//current_dir = current_dir;
	}	

	while (true) {
		char input[4096];
		int num_programs = 1;
		
		printf("[%s]# ", current_dir);
		if (fgets(input, 4096, stdin) == NULL) { //use fgets to store the command in a string
			return -1;
		}
	
		input[strcspn(input, "\n")] = 0;

		char *curr_program = strtok(input, "|");
		char *next_program;
		int other_side = 0;
		int pipefd[2];
		bool piping = false;

		while ((next_program = strtok(NULL, "|"))) {

			//always gonna pipe because we are in while loop until *programs != NULL. Will only equal null when no pipes left
			if (pipe(pipefd) < 0) {
				perror("pipe() error");
			}
			piping = true;

			other_side = execute_program(curr_program, other_side, pipefd, piping);
			
			curr_program = next_program;
			num_programs++;
		}

		piping = false;
		other_side = execute_program(curr_program, other_side, pipefd, piping);
		for (int i = 0; i < num_programs; i++) {
			if (wait(NULL) < 0) {
				perror("wait() error");
			}
		}
	}
}

int execute_program(char *curr_program, int other_side, int pipefd[], bool piping) {
	int redir_fd;
	bool output_re = false;
	bool input_re = false;
	bool append = false;
	char *file;
	int out_fd = 1;
	pid_t child_pid;

	if (strncmp(curr_program, "exit", 4) == 0 && !piping) { 
		exit(0);
	}

	if ((child_pid = fork()) < 0) {
		perror("fork() error");
	}
	else if (child_pid == 0) { //child process	
		char *args_in_prog[10];
		char *commargs[10];
		int commcount = 0;
		char *curr_arg = strtok(curr_program, " ");
		int num_args = 0;
		while(curr_arg != NULL) {
			args_in_prog[num_args] = curr_arg;
			curr_arg = strtok(NULL, " ");
			num_args++;
		}
		args_in_prog[num_args] = NULL;
			
		for (int i = 0; i < num_args; i++) {
			switch (*args_in_prog[i]) {
				case '>':
					//if it isn't >> output redirection
					if (strlen(args_in_prog[i]) == 1) {
						output_re = true;
						file = args_in_prog[i + 1];	
					}
					else { //if it is >>
						append = true;
						file = args_in_prog[i + 1];	
						i++; //I think this would be necessary to skip the next '>'
					}
					break;
				case '<':
					input_re = true;
					file = args_in_prog[i + 1];
					break;
				default:
					if (output_re || input_re || append) {
						goto exitloop;
					}
					commargs[commcount] = args_in_prog[i];
					commcount += 1;
					break;
			}
		}
		exitloop: ;
		
		commargs[commcount] = NULL;
		
		if (piping) {
			out_fd = pipefd[1];

			if (dup2(out_fd, 1) < 0) {
				perror("dup2() error");
			}
		}	
		if (dup2(other_side, 0) < 0) {
			perror("dup2() error");
		}
		if (out_fd != 1) {
			close(out_fd);
		}
		if (output_re) {
			if ((redir_fd = open(file, O_WRONLY | O_CREAT, 0777)) < 0) {
				perror("open() error");
				exit(42);
			}
			if (dup2(redir_fd, 1) < 0) {
				perror("dup2() error");
				exit(42);
			}
			if (close(redir_fd) < 0) {
				perror("close() error");
				exit(42);
			}
		}
		if (input_re) {
			if ((redir_fd = open(file, O_RDONLY | O_CREAT, 0777)) < 0) {
				perror("open() error");
				exit(42);
			}
			if (dup2(redir_fd, 0) < 0) {
				perror("dup2() error");
				exit(42);
			}
			if (close(redir_fd) < 0) {
				perror("close() error");
				exit(42);
			}
		}
		if (append) {
			if ((redir_fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0777)) < 0) {
				perror("open() error");
				exit(42);
			}
			if (dup2(redir_fd, 1) < 0) {
				perror("dup2() error");
				exit(42);
			}
			if (close(redir_fd) < 0) {
				perror("close() error");
				exit(42);
			}
		}
		if ((execvp(args_in_prog[0], commargs)) < 0) {
			perror("Not a valid command");
			errno = ENOENT;
			exit(42);
		}
	}
	else { //parent process
		if (piping) {
			close(pipefd[1]);
		}
		if (out_fd != 1) {
			if (close(out_fd) < 0) {
				perror("close() error");
			}
		}
		if (piping) {
			other_side = pipefd[0];
		}
	}

	return other_side;
}
