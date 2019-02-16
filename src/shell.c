#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

void cmdExec();

char **get_command(char *cmd);

int cd(char *);

int main(void) {
    
    while (1) {
        cmdExec();
    }
    return 0;
    
}

void cmdExec(){
    size_t len = 0;
	ssize_t read = 0;
	char *cmd;
	char **args;
    int stat_loc;
    pid_t child_pid;

    
    read = 0;
    printf("> ");
    read = getline(&cmd, &len, stdin);
    args = get_command(cmd);
        
    // Empty command
    if (!args[0]) {
        free(cmd);
        free(args);
        return;
    }
    
    // handel cd cases
    if (strcmp(args[0], "cd") == 0) {
        if (cd(args[1]) < 0) {
            perror(args[1]);
        }
        return;
    }

    child_pid = fork();
    // call not successful
    if (child_pid == 0) {
        execvp(args[0], args);
        printf("execvp not successful\n");
    } else {
        waitpid(child_pid, &stat_loc, WUNTRACED);
    }
    free(cmd);
    free(args);
    
}

char **get_command(char * cmd) {
	char **args = malloc(8*sizeof(char*));
	const char *sep = " \n";
	char *token;
	int i = 0; 

	token = strtok(cmd, sep);
	while (token != NULL) {
    	args[i] = strdup(token);
        token = strtok(NULL, sep);
		i++;
	} 	
	args[i] = NULL;
	return args;
}

// Handle cd cases
int cd(char *path) {
    return chdir(path);
}
