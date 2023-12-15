#include "mysh.h"

#define DELIMS " <>|"

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

		printf("[%s]# ", current_dir);
		if (fgets(input, 4096, stdin) == NULL) { //use fgets to store the command in a string
			return -1;
		}
	
		input[strcspn(input, "\n")] = 0;

		struct block {
			int num_args;
			int num_comm;
			char *args[12];
			char *commargs[12];
			bool output_re;
			bool append;
			bool input_re;
			char *file;
		};

		char *arg_blocks[12]; //might need to be block_num + 1 because array needs to be NULL terminated	
		
		char *pipe_tok = strtok(input, "|");
		int num_blocks = 0;

		while (pipe_tok != NULL) {
			arg_blocks[num_blocks] = pipe_tok;
			pipe_tok = strtok(NULL, "|");
			num_blocks++;
		}

		struct block *final_blocks[12];

		for (int i = 0; i < 12; i++) {
			struct block *new_block = malloc(sizeof(struct block)); //need to allocate mem for this
			new_block->num_args = 0;
			new_block->num_comm = 0;
			new_block->output_re = false;
			new_block->append = false;
			new_block->input_re = false;
			final_blocks[i] = new_block;
		}

		char *space_tok;
		
		char *args[12];
		int argcount = 0;	

		for (int i = 0; i < num_blocks; i++) {
			space_tok = strtok(arg_blocks[i], " ");
			int args_in_blk = final_blocks[i]->num_args;
			while(space_tok != NULL) {
				args[argcount] = space_tok;
				final_blocks[i]->args[args_in_blk] = space_tok;
				space_tok = strtok(NULL, " ");

				argcount++;
				args_in_blk++;
			}
			final_blocks[i]->num_args = args_in_blk;
			final_blocks[i]->args[args_in_blk] = NULL;
		}

		args[argcount] = NULL; //want to null terminate the arguments
		arg_blocks[num_blocks] = NULL;

		int numPipes;
		int x = 0;

		for (int i = 0; i < argcount; i++) {
			if(*args[i] == '|') {
				numPipes += 1;
				x++;
				
			}
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
			
			if ((child_pid = fork()) < 0) {
				perror("fork() error");
			}
			else if (child_pid == 0) { //child process
				
				if (dup2(out_fd, 1) < 0) {
					perror("dup2() error");
				}
				if (dup2(other_side, 0) < 0) {
					perror("dup2() error");
				}
				if (out_fd != 1) {
					close(out_fd);
				}
				//close(other_side);

				if (final_blocks[i]->output_re) {
					printf("output re\n");
					if ((redir_fd = open(final_blocks[i]->file, O_RDWR | O_CREAT, 0777)) < 0) {
						perror("open() error");
					}
					printf("redir_fd: %d\n", redir_fd);
					if (dup2(redir_fd, 1) < 0) {
						perror("dup2() error");
					}
					if (close(redir_fd) < 0) {
						perror("close() error");
					}
				}
				if (final_blocks[i]->input_re) {
					if ((redir_fd = open(final_blocks[i]->file, O_RDWR | O_CREAT, 0777)) < 0) {
						perror("open() error");
					}
					if (dup2(redir_fd, 0) < 0) {
						perror("dup2() error");
					}
					if (close(redir_fd) < 0) {
						perror("close() error");
					}
				}
				if (final_blocks[i]->append) {
					printf("append re\n");
					if ((redir_fd = open(final_blocks[i]->file, O_RDWR | O_CREAT | O_APPEND, 0777)) < 0) {
						perror("open() error");
					}
					if (dup2(redir_fd, 1) < 0) {
						perror("dup2() error");
					}
					if (close(redir_fd) < 0) {
						perror("close() error");
					}
				}
				execvp(final_blocks[i]->args[0], final_blocks[i]->commargs);
			}
			else { //parent process		
				//close(pipefd[1]); //close the write end of the first pipe because the child should already have it
				if (out_fd != 1) {
					if (close(out_fd) < 0) {
						perror("close() error");
					}
				}
				//close(other_side);
				if (wait(NULL) < 0) {
					perror("wait() error");
				}
				//close(out_fd);
			//	close(out_fd);
				other_side = pipefd[0];
			}
		}
	}
}


