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


	//while (true) {
	char *comm = malloc(4096);

	printf("[%s]# ", current_dir);
	scanf("%s", comm);
	

	char *buf = "hello my friend";

	char *tok = strtok(comm, " ");

	char *args[3];
	int count = 0;
	while (tok != NULL) {
		args[count++] = tok;
		tok = strtok(NULL, " ");
	}

	printf("%s\n", args[1]);
	printf("%s\n", args[2]);


	//parse_line(args);

	//}

	/*
	char *args[argc - 1];;
	for (int i = 0; i < argc; i++) {
		args[i] = argv[i + 1];
	}

	parse_line(comm, args);
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
