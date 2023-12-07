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

		char *commargs[argcount];
		int commargcount = 0;

		//printf("before that\n");


		//we want to create a list of just the arguments relevant to the ls command (so not I/O redirection)
		for (int j = 0; j < argcount; j++) {
			//printf("breaking immediately?\n");
			if (*args[j] == '<' || *args[j] == '>' || *args[j] == '|') {
				commargs[j] = NULL;
				break;
			}
			else {
				commargs[j] = args[j];
			}
			commargcount++;
		}

		commargs[commargcount] = NULL;
		//might later want to break it down even more into things on either side of a pipe and have it recursively do piping kind of
		
		//this is just if there is no io redirection
		//execvp(cmd, commargs);
		
		for (int n = 0; n < commargcount; n++) {
			printf("commargs: %s\n", commargs[n]);
		}

		int pipeint[2] = {3, 4};

		for (int i = 0; i < argcount; i++) {
			if (*args[i] == '|') {
				char *pipe_fill = args[i-1];
				char *pipe_drain = args[i+1];
				//printf("fill: %s drain: %s\n", pipe_fill, pipe_drain);
				pipe(pipeint);
		
				child_pid = fork();
				close(4);
				child_pid2 = fork();
				close(3);

				if (child_pid == 0) {
					printf("ls process\n");
					close(4);
					close(3);
					exit(42);
				}
				else if (child_pid2 == 0) {
					printf("grep process\n");
					close(3);
					exit(42);
				}
				printf("parent process before wait\n");	
				wait(NULL);
				printf("parent process between\n");
				wait(NULL);
				printf("parent process after\n");
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
			execvp(cmd, commargs);
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
