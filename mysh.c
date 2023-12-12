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
		

		char buf[] = "hello my friend";

		char *tok = strtok(input, " ");
		char *cmd = tok;


		char *args[12];
		//args[0] = tok;
		int argcount = 0;
		while (tok != NULL) {
			args[argcount] = tok;
			tok = strtok(NULL, " ");

			argcount++;
		}
		args[argcount] = NULL; //want to null terminate the arguments
		
		int delim_loc = 1; //index of where first I/O redirect char is
		//for (int j = 1; j < argcount; j++) {
			
		//}

		char *lastarg = args[argcount - 1];
		lastarg[strcspn(lastarg, "\n")] = 0;

		pid_t child_pid, child_pid2, exit_pid, exit_pid2;
		int exit_value, exit_value2;

		//char *commargs[argcount];
		int commargcount = 0;
		int delim_count = 0;
		int block_num = 0;
		
		struct cmd_block {
			int arg_num;
			char* arg_array[12];
		};
		//printf("before that\n");

		//we want to create a list of just the arguments relevant to the ls command (so not I/O redirection)
		for (int i = 0; i < 12; i++) {
			if (*args[i] == '<' || *args[i] == '>' || *args[i] == '|') {
				delim_count++;

			}	
		}

		block_num = delim_count+1;

		struct cmd_block *block_array[block_num + 1]; //might need to be block_num + 1 because array needs to be NULL terminated	
		for (int i = 0; i < block_num + 1; i++) {
			struct cmd_block *new_block; //need to allocate mem for this
			block_array[i] = new_block;
		}

		block_array[block_num] = NULL;

		for (int i = 0; i < block_num; i++) {
			char* temp_array[12];
			for (int j = 0; j < argcount; j++) {
				if (*args[j] == '<' || *args[j] == '>' || *args[j] == '|') {
			
					block_num += 1;
				}
				else {
					temp_array[j] = args[j];
					block_array[i]->arg_num += 1;
				}
				
			}
			//block_array[i]->arg_array[] = temp_array[];
			i++;
		}

		for (int i = 0; block_num+1; i++) {
			for (int j = 0; j < block_array[i]->arg_num; j++) {
				printf("%s\n", block_array[i]->arg_array[j]);
			}
		}

		//might later want to break it down even more into things on either side of a pipe and have it recursively do piping kind of
		
		//this is just if there is no io redirection
		//execvp(cmd, commargs);


		//int pipeint[2] = {3, 4};
		int numPipes;
		int pipe_Indices[5];
		int x = 0;
			
		for (int i = 0; i < argcount; i++) {
			if(*args[i] == '|') {
				numPipes += 1;
				pipe_Indices[x] = i;
				x++;
				
			}
		}

		int pipefds[2*numPipes];

		for (int i = 0; i < numPipes; i++) {
			pipe(pipefds + i*2);

		}

		// ls | grep m | sort
	
		//is there a way to find out what file descriptor the process *pipe_fill writes to and have it be a variable (I dont believe this is necessary anymore)
		//How do we make it so we can have multiple pipes?
		//Do we have to worry about if a command does not exist: Yes.
		//Need a case for if args != 2, or a case where the pipe does not have an input or an output
		//ls | grep m is doing ls and ls | grep m and i dont know why. FIXED

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
					if(execvp(pipe_fill, commargs) == -1) {
						printf("Command not found\n");
						exit(1);
					}
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
		
		if ((child_pid = fork()) < 0) {
			perror("fork() error");
		}
		else if (child_pid == 0) {
			//printf("child executing\n");
			
			for (int i = 1; i < argcount; i++) {
				int fd;
				switch (*args[i]) { //i think this should be fine we only care about > >> < and | and those are all just one character or can be compared to just one more char
					//we want to create a list of just the arguments relevant to the ls command (so not I/O redirection)
					case '>':
						//printf("Case >\n");
						//if it isn't >> output redirection
						if (*args[i + 1] != '>') {
							//printf("here\n");
							fd = open(args[i + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
						}
						else { //if it is >>
							fd = open(args[i + 2], O_RDWR | O_APPEND | O_CREAT, S_IRUSR | S_IWUSR);
							i++; //I think this would be necessary to skip the next '>'
						}
						dup2(fd, 1);
						//execvp(cmd, args);
						//

						break;
					case '<':
						//printf("Case <\n");
						fd = open(args[i + 1], O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
						dup2(fd, 0);
						//execvp(cmd, args);
						break;
					default:
						//printf("no action necessary\n");
						break;
				}

			}
			
			//printf("bout to exec\n");
			//execvp(cmd, commargs);
			exit(42);
		}
		else { //shell process waits for the child (the command the user gave) to terminate

			if ((exit_pid = wait(&exit_value)) == -1) {
				perror("wait() error");
			}
			//how do I deal with the error here
		}
		
	
	
	}


			/*
			for (int i = 1; i < argcount; i++) {
				int fd;
				//printf("%c\n", args[i][0]);
				if (*args[i] == '>') {
					//if it isn't >> output redirection
					if (*args[i + 1] != '>') {
						fd = open(args[i + 1], O_RDWR | O_CREAT);
					}
					else { //if it is >>
						fd = open(args[i + 2], O_RDWR | O_APPEND | O_CREAT);
						i++; //I think this would be necessary to skip the next '>'
					}
					dup2(fd, 1);
				}
				else if (*args[i] == '<') {
					fd = open(args[i + 1], O_RDWR);
					dup2(fd, 0);
				}
				else {
					printf("nothing\n");
					break;
				}
			}*/


	/*
	 				switch (args[i][0]) { //i think this should be fine we only care about > >> < and | and those are all just one character or can be compared to just one more char
					case '>':
						//if it isn't >> output redirection
						if (args[i + 1][0] != '>') {
							fd = open(args[i + 1], O_RDWR | O_CREAT);
						}
						else { //if it is >>
							fd = open(args[i + 2], O_RDWR | O_APPEND | O_CREAT);
							i++; //I think this would be necessary to skip the next '>'
						}
						dup2(fd, 1);
						//execvp(cmd, args);
						break;
					case '<':
						fd = open(args[i + 1], O_RDWR);
						dup2(fd, 0);
						//execvp(cmd, args);
						break;
					default:
						printf("no action necessary\n");
						break;

				}

*/

	
}

char **parse_line(char *args[]) {

	printf("%s\n", args[0]);
	printf("%s\n", args[1]);
	printf("%s\n", args[2]);
	/*
	for (int i = 0; i < 3; i++) {
		printf("%s\n", args[i]);
	}
	*/

	return args;
}
