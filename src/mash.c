#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_CMD_NUM 3
char **get_command(char *cmd);

int cd(char *);

void func(char *src, char *sub);
    
int exe();

int main(void) {
    // while (1) {
        exe();
    // }
    return 0;
    
}

int exe() {
    size_t len, write = 0;
	ssize_t read = 0;
	char **cmd = (char**)malloc(sizeof(char*)*MAX_CMD_NUM);
    char *file = (char*)malloc(sizeof(char)*len);
	char ***args = (char***)malloc(sizeof(char**)*MAX_CMD_NUM);;
    int stat_loc[MAX_CMD_NUM], i;
    pid_t child_pid[MAX_CMD_NUM];
    read = 0;
    for(i = 0; i < MAX_CMD_NUM; i++) {
        printf("mash-%d>", (i + 1));
        read = getline(&cmd[i], &write, stdin);
        args[i] = get_command(cmd[i]);
    
        // Empty command
        if (!args[i][0]) {
            free(cmd[i]);
            free(args[i]);
            return -1;
        }
        
        // handel cd cases
        if (strcmp(args[i][0], "cd") == 0) {
            if (cd(args[i][1]) < 0) {
                perror(args[i][1]);
            }
            return -1;
        }
    }
    printf("file>");
    read = getline(&file, &write, stdin);
        
    
    child_pid[0] = fork();
    printf("cmd: 1\n\tinput1: %s\n\tPID1:%d\n", cmd[0], (int) getpid());
    if (child_pid[0] == 0) {
        execvp(args[0][0], args[0]);
        printf("execvp not successful\n");
    } else {
        child_pid[1] = fork();
        printf("cmd: 2\n\tinput2: %s\n\tPID2:%d\n", cmd[1], (int) getpid());
        if (child_pid[1] == 0) {
            waitpid(child_pid[2], &stat_loc[2], WUNTRACED);
            execvp(args[1][0], args[1]);
            printf("execvp not successful\n");
        } else {            
            waitpid(child_pid[2], &stat_loc[2], WUNTRACED);
            child_pid[2] = fork();
            printf("cmd: 3\n\tinput3: %s\n\tPID3:%d\n", cmd[2], (int) getpid());
            if (child_pid[2] == 0) {
                waitpid(child_pid[2], &stat_loc[2], WUNTRACED);
                execvp(args[2][0], args[2]);
                printf("execvp not successful\n");
            } else {
                // wait(NULL);
                waitpid(child_pid[2], &stat_loc[2], WUNTRACED);
                printf("Third process finished...\n");            
            }
            // wait(NULL);
            waitpid(child_pid[1], &stat_loc[1], WUNTRACED);
            printf("Second process finished...\n");            
        }
        // wait(NULL);
        waitpid(child_pid[0], &stat_loc[0], WUNTRACED);
        printf("First process finished...\n");            
    }
    free(cmd);
    free(args);
    return 0;
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
