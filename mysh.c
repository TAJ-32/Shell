#include "mysh.h"

#define DELIMS " <>|"

int execute_program(char *curr_program, int other_side);

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
		pid_t child_pid;
		while ((next_program = strtok(NULL, "|"))) {	
			int redir_fd;
			bool output_re = false;
			bool input_re = false;
			bool append = false;
			char *file;
			int pipefd[2];
			int out_fd = 1;

			//always gonna pipe because we are in while loop until *programs != NULL. Will only equal null when no pipes left
			if (pipe(pipefd) < 0) {
				perror("pipe() error");
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

				if (strcmp(args_in_prog[0], "exit") == 0) { 
					return 0;
				}
					
				for (int i = 0; i < num_args; i++) {
					switch (*args_in_prog[i]) {
						case '<':
							input_re = true;
							file = args_in_prog[i + 1];
							break;
						default:
							if (output_re || input_re || append) {
								goto exit_loop;
							}

							commargs[commcount] = args_in_prog[i];
							commcount += 1;
							break;
					}
				}
				exit_loop: ;
				
				commargs[commcount] = NULL;

				out_fd = pipefd[1];

				if (dup2(out_fd, 1) < 0) {
					perror("dup2() error");
				}
				if (dup2(other_side, 0) < 0) {
					perror("dup2() error");
				}
				if (out_fd != 1) {
					close(out_fd);
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
				if ((execvp(args_in_prog[0], commargs)) < 0) {
					perror("Not a valid command");
					errno = ENOENT;
					exit(42);
				}
			}
			else { //parent process
				close(pipefd[1]);
				if (out_fd != 1) {
					if (close(out_fd) < 0) {
						perror("close() error");
					}
				}

				other_side = pipefd[0];
			}

			
			curr_program = next_program;
			num_programs++;
		}


		int redir_fd;
		bool output_re = false;
		bool input_re = false;
		bool append = false;
		char *file;
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

			if (strcmp(args_in_prog[0], "exit") == 0) { 
				return 0;
			}
				
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
							goto exitloop2;
						}
						commargs[commcount] = args_in_prog[i];
						commcount += 1;
						break;
				}
			}
			exitloop2: ;
			
			commargs[commcount] = NULL;

			if (dup2(other_side, 0) < 0) {
				perror("dup2() error");
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

		}



		for (int i = 0; i < num_programs; i++) {
			if (wait(NULL) < 0) {
				perror("wait() error");
			}
			//free(final_blocks[i]);
		}
	}
}

int execute_program(char *curr_program, int other_side) {
	int redir_fd;
	bool output_re = false;
	bool input_re = false;
	bool append = false;
	char *file;
	int pipefd[2];
	int out_fd = 1;

//always gonna pipe because we are in while loop until *programs != NULL. Will only equal null when no pipes left
	if (pipe(pipefd) < 0) {
		perror("pipe() error");
	}
	pid_t child_pid;

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

		if (strcmp(args_in_prog[0], "exit") == 0) { 
			return 0;
		}
			
		for (int i = 0; i < num_args; i++) {
			printf("arg %d: %s\n", i, args_in_prog[i]);
			switch (*args_in_prog[i]) {
				case '>':
					//if it isn't >> output redirection
					if (strlen(curr_arg) == 1) {
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
					printf("input\n");
					input_re = true;
					file = args_in_prog[i + 1];
					break;
				default:
					if (output_re || input_re || append) {
						goto exit_loop;
					}

					commargs[commcount] = args_in_prog[i];
					commcount += 1;
					break;
			}
		}
		exit_loop: ;
		
		commargs[commcount] = NULL;

		out_fd = pipefd[1];

		if (dup2(out_fd, 1) < 0) {
			perror("dup2() error");
		}
		printf("other_side: %d\n", other_side);
		if (dup2(other_side, 0) < 0) {
			perror("dup2() error");
		}
		if (out_fd != 1) {
			close(out_fd);
		}

		if (output_re) {
			printf("output re\n");
			if ((redir_fd = open(file, O_WRONLY | O_CREAT, 0777)) < 0) {
				perror("open() error");
				exit(42);
			}
			printf("redir_fd: %d\n", redir_fd);
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
			printf("append re\n");
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
		if (out_fd != 1) {
			if (close(out_fd) < 0) {
				perror("close() error");
			}
		}

		other_side = pipefd[0];
	}

	return other_side;
}


		/*
		char *space_tok;
		
		//char *args[12];
		int argcount = 0;	

		for (int i = 0; i < num_blocks; i++) {
			space_tok = strtok(arg_blocks[i], " ");
			int args_in_blk = final_blocks[i]->num_args;
			while(space_tok != NULL) {
				//args[argcount] = space_tok;
				final_blocks[i]->args[args_in_blk] = space_tok;
				space_tok = strtok(NULL, " ");

				argcount++;
				args_in_blk++;
			}
			final_blocks[i]->num_args = args_in_blk;
			final_blocks[i]->args[args_in_blk] = NULL;
		}

		//args[argcount] = NULL; //want to null terminate the arguments
		arg_blocks[num_blocks] = NULL;

		if (strcmp(final_blocks[0]->args[0], "exit") == 0) {
			return 0;
		}

		for (int i = 0; i < num_blocks; i++) {
			int commcount = 0;// = final_blocks[i]->num_comm;
			for (int j = 0; j < final_blocks[i]->num_args; j++) {
				commcount = final_blocks[i]->num_comm;
				switch (*final_blocks[i]->args[j]) {
					case '>':
						//if it isn't >> output redirection
						if (strlen(final_blocks[i]->args[j]) == 1) {
							final_blocks[i]->output_re = true;
							final_blocks[i]->file = final_blocks[i]->args[j + 1];	
						}
						else { //if it is >>
							final_blocks[i]->append = true;
							final_blocks[i]->file = final_blocks[i]->args[j + 1];	
							j++; //I think this would be necessary to skip the next '>'
						}
						break;
					case '<':
						final_blocks[i]->input_re = true;
						final_blocks[i]->file = final_blocks[i]->args[j + 1];
						break;
					default:
						if (final_blocks[i]->output_re || final_blocks[i]->input_re || final_blocks[i]->append) {
							goto exit_loop;
						}
						final_blocks[i]->commargs[commcount] = final_blocks[i]->args[j];
						final_blocks[i]->num_comm += 1;
						break;
				}
			}
			exit_loop: ;
		}

		pid_t child_pid;
		//this won't run if there is one block only, which is what we want, we just want to exec. Also, we are already in the child process. Which we also want.
		int other_side = 0;
		for (int i = 0; i < num_blocks; i++) {
			int pipefd[2];
			int out_fd = 1;
			int redir_fd;

			if (i != num_blocks - 1) {
				if (pipe(pipefd) < 0) {
					perror("pipe() error");
				}
				out_fd = pipefd[1];
			}
		
			//do all the forking first and then all the waiting	
		}
	*/

		/*
		for (int i = 0; i < num_blocks; i++) {
			if (wait(NULL) < 0) {
				perror("wait() error");
			}
			//free(final_blocks[i]);
		}*/
