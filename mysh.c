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
		fgets(input, 4096, stdin); //use fgets to store the command in a string
	
		input[strcspn(input, "\n")] = 0;

		char buf[] = "hello my friend";

		//strtok the pipes


		//char *arg_blocks[12];

		struct block {
			int num_args;
			char *args[12];
			char *commargs[12];
		};

		char *arg_blocks[12]; //might need to be block_num + 1 because array needs to be NULL terminated	




		//how much of the parsing should I be doing in the child
		
		char *pipe_tok = strtok(input, "|");
		int num_blocks = 0;

		while (pipe_tok != NULL) {
			arg_blocks[num_blocks] = pipe_tok;
			pipe_tok = strtok(NULL, "|");
			num_blocks++;
		}

		for (int i = 0; i < num_blocks; i++) {
			printf("blocks: %s\n", arg_blocks[i]);
		}


		struct block *final_blocks[12];

		for (int i = 0; i < 12; i++) {
			struct block *new_block = malloc(sizeof(struct block)); //need to allocate mem for this
			new_block->num_args = 0;
			final_blocks[i] = new_block;
		}

		char *space_tok;
	       	char *cmd = space_tok;	
		
		char *args[12];
		int argcount = 0;	
		int block_count = 0;

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


		//int pipeint[2] = {3, 4};
		int numPipes;
		int pipe_Indices[5];
		int x = 0;

		for (int i = 0; i < num_blocks; i++) {
			printf("BLOCK %d: [", i);
			for (int j = 0; j < final_blocks[i]->num_args; j++) {
				printf("%s,", final_blocks[i]->args[j]);
			}
			printf("]\n");
		}



		pid_t child_pid, child_pid2, exit_pid, exit_pid2;
		int exit_value, exit_value2;

		if ((child_pid = fork()) < 0) {
			perror("fork() error");
		}
		else if (child_pid == 0) {

			//why start making the blocks here? Doesn't it make more sense to have the entire line the user input
			//and then go through each part of each block, forking as we go along for any process we see,
			//redirecting output for any > or < we see, and piping for any | we see?
			//
			//For example: We have ls -l < myfile.txt | grep m;
			//	The first block is [ls, -l, <, myfile.txt]
			//	The second block is [grep, m]
			//	If there is more than one block, that means we are going to need to pipe
			//	We loop through each block one at a time, forking at the beginning of the loop to create a child for this block to run 
			//		its process because every block will have a process to run, even if no pipe is gonna happen
			//	We check if a pipe is going to follow this block. We do that by knowing there is one less pipe than there are blocks. So if
			//		we are on the first block of two, we know we are writing into a pipe. If we are on the second block of three,
			//		we are reading from a pipe and writing to a different one, if we are on the third block of four, same thing, etc.
			//		based on that, we can know if we will need to pipe once or twice in this iteration of a loop
			//	Regardless, we loop through our first block and see a <
			//	Everything before that <, which is [ls, -l] gets exec'ed. 
			//	We can maybe save that in a separate temp array within the child and then exec with it
			//	The rest of the block we also still have access to and so we can open whatever comes after the <, which is myfile.txt
			//	All of this is still happening in the child.
			//	We dup2(fd, 1). If we are piping into something, we also dup2(pipeint[1], 1).
			//	If we are draining from a pipe, we also do dup2(pipeint[0], 0)
			//	Then we execvp(temp_array[0], temp_array);

			for (int i = 0; i < num_blocks; i++) {
				for (int j = 0; j < final_blocks[i]->num_args; j++) {
					int fd;
					switch (*final_blocks[i]->args[j]) {
						case '>':
							//printf("Case >\n");
							//if it isn't >> output redirection
							if (*final_blocks[i]->args[j + 1] != '>') {
								//printf("here\n");
								fd = open(final_blocks[i]->args[j + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
							}
							else { //if it is >>
								fd = open(final_blocks[i]->args[j + 2], O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
								j++; //I think this would be necessary to skip the next '>'
							}
							dup2(fd, 1);
							//execvp(cmd, args);
							//

							break;
						case '<':
							//printf("Case <\n");
							fd = open(final_blocks[i]->args[j + 2], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
							dup2(fd, 0);
							//execvp(cmd, args);
							break;
						default:
							final_blocks[i]->commargs[j] = args[j];
							//printf("no action necessary\n");
							break;
					}

				}
			}
		}
		else { //shell process waits for the child (the command the user gave) to terminate

			if ((exit_pid = wait(&exit_value)) == -1) {
				perror("wait() error");
			}
			//how do I deal with the error here
		}

	}
}


/*
		for (int i = 0; i < argcount; i++) {
			if(*args[i] == '|') {
				numPipes += 1;
				pipe_Indices[x] = i;
				x++;
				
			}
		}
		*/




	

		/*
		for (int i = 0; i < argcount; i++) {
			if (*args[i] == '|') {
				char *pipe_fill = cmd;
				char *pipe_drain = args[i+1];
				//printf("fill: %s drain: %s\n", pipe_fill, pipe_drain);
				//pipe(pipeint);
		
				child_pid = fork();
				if (child_pid == 0) {
					//printf("ls process\n");
					if (dup2(4,1) < 0) {
						perror("dup2");
						exit(1);
					}
					close(4);
					close(3);
					printf("cmd is: %s\n", cmd);
					//printf("%s\n", commargs);
					//if(execvp(pipe_fill, commargs) == -1) {
					//	printf("Command not found\n");
					//	exit(1);
					//}
					
				}
				close(4);
				child_pid2 = fork();
				if (child_pid2 == 0) {
					//printf("grep process\n");
					if(dup2(3,0) < 0) {
						perror("dup2");
						exit(1);
					}
					close(3);
					
					char *pipe_drain_args[12];

					int j;
					int x;
					int count = 0;
					for (j = i + 1, x = 0; j < argcount; j++, x++) {
						pipe_drain_args[x] = args[j];
					       	//printf("yeah: %s\n", pipe_drain_args[x]);
						count++;	
					}
					pipe_drain_args[count] = NULL;
					
					execvp(pipe_drain, pipe_drain_args); 
					exit(0);
				}
				close(3);	
				wait(NULL);
				wait(NULL);
			}
		}

		//this handles the I/O redirection		
	}
*/


