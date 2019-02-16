#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int main(void) {
	size_t len = 0;
	ssize_t read = 0;
	char *input;
	char **args = malloc(8*sizeof(char*));
	const char *sep = " ";
	char *token;
	int i = 0; 

	printf("> ");
	read = getline(&input, &len, stdin);
	token = strtok(input, " \n");
	printf("%s\n", token);
	while (token != NULL) {
		args[i] = strdup(token);
		printf("%s\n", token);
		token = strtok(NULL, " \n");
		i++;
	} 	
	args[i] = NULL;
 
	execvp(args[0], args);

	free(args);
	return 0;
}
