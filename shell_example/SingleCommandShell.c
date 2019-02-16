#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char **get_command(char * input);

int main(void) {
	size_t len = 0;
	ssize_t read = 0;
	char *input;
	char **args;
	
	printf("> ");
	read = getline(&input, &len, stdin);
	
	args = get_command(input);
	execvp(args[0], args);

	free(args);
	return 0;
}


char **get_command(char * input) {
	char **args = malloc(8*sizeof(char*));
	const char *sep = " \n";
	char *token;
	int i = 0; 

	token = strtok(input, sep);
	while (token != NULL) {
		args[i] = strdup(token);
		token = strtok(NULL, sep);
		i++;
	} 	
	args[i] = NULL;
	return args;
}
