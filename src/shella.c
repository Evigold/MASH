#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

int exe();

char **get_command(int i);

int cd(char *);

int main(void) {
    exe();
    return 0;
    
}

int exe() {
    char ***args = (char***)malloc(sizeof(char**)*3);;
    int stat_loc[3], i;
    pid_t child_pid[3], check[3];
    
    for(i = 0; i < 3; i++) {
        args[i] = get_command(i);
    }

    child_pid[0] = fork();
   
    if (child_pid[0] == 0) {
        execvp(args[0][0], args[0]);
        printf("execvp not successful\n");
    } else {
        
        child_pid[1] = fork();
        
        if (child_pid[1] == 0) {
            execvp(args[1][0], args[1]);
            printf("execvp not successful\n");
        } else {            
            
            child_pid[2] = fork();
            
            if (child_pid[2] == 0) {
                execvp(args[2][0], args[2]);
                printf("execvp not successful\n");
            } else {
                check[2] = waitpid(child_pid[2], &stat_loc[2], WUNTRACED);
                printf("Third process finished...%d-%d\n", (int) check[2], (int) child_pid[2]);            
            }
            check[1] = waitpid(child_pid[1], &stat_loc[1], WUNTRACED);
            printf("Second process finished...%d\n", (int) check[1]);            
        }
        check[0] = waitpid(child_pid[0], &stat_loc[0], WUNTRACED);
        printf("First process finished...%d\n", (int) check[0]);            
    }
    
    for(i = 0; i < 3; i++) {
        if(check[i] == child_pid[i]) {
            //TODO - print from pipe
        }
    }

    free(args);
    return 0;
}

char **get_command(int num) {
	size_t len, write = 0;
	ssize_t read = 0;
    char **args = malloc(8*sizeof(char*));
	char * cmd = (char*)malloc(sizeof(char)*len);
    const char *sep = " \n";
	char *token;
	int i = 0; 
	printf("mash-%d>", (num + 1));
    read = getline(&cmd, &write, stdin);
        
	token = strtok(cmd, sep);
	while (token != NULL) {
    	args[i] = strdup(token);
        token = strtok(NULL, sep);
		i++;
	} 	
	args[i] = NULL;
    // Empty command
    if (!args[0]) {
        free(cmd);
        free(args);
        return NULL;
    }
    
    // handel cd cases
    if (strcmp(args[0], "cd") == 0) {
        if (cd(args[1]) < 0) {
            perror(args[1]);
        }
        return args;
    }
    free(cmd);
	return args;
}

// Handle cd cases
int cd(char *path) {
    return chdir(path);
}
