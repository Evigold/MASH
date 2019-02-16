#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

char **get_command(char *cmd);

int cd(char *);

void func(char *src, char *sub);
    
int exe(int cmdNum);

int main(void) {
    while (1) {
        exe(1);
    }
    return 0;
    
}

int exe(int cmdNum) {
    size_t len = 0;
	ssize_t read = 0;
	char *cmd;
	char **args1;
    int stat_loc;
    pid_t child_pid;
    read = 0;
    printf("mash-%d>", cmdNum);
    read = getline(&cmd, &len, stdin);
    args1 = get_command(cmd);
        
    // Empty command
    if (!args1[0]) {
        free(cmd);
        // free(args1);
        return -1;
    }
    
    // handel cd cases
    if (strcmp(args1[0], "cd") == 0) {
        if (cd(args1[1]) < 0) {
            perror(args1[1]);
        }
        return -1;
    }


    child_pid = fork();
    if (child_pid == 0) {
        execvp(args1[0], args1);
        printf("execvp not successful\n");
    } else {
        if (cmdNum < 4) {
            printf("im here\n");
            exe(cmdNum + 1);
        }
        waitpid(child_pid, &stat_loc, WUNTRACED);
    }
    free(cmd);
    free(args1);
    return 0;
}

char **get_command(char * cmd) {
	char **args1 = malloc(8*sizeof(char*));
	const char *sep = " \n";
	char *token;
	int i = 0; 

	token = strtok(cmd, sep);
	while (token != NULL) {
    	args1[i] = strdup(token);
        token = strtok(NULL, sep);
		i++;
	} 	
	args1[i] = NULL;
	return args1;
}

// Handle cd cases
int cd(char *path) {
    return chdir(path);
}
