#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

int exe();

char **get_command(int i);

int cd(char *);

int main(void) {
    exe();
    return 0;
    
}

int exe() {
    struct timeval start[3], stop[3];
    size_t len, write;
    ssize_t readline = 0;
    char ***args = (char***)malloc(sizeof(char**)*3);;
    pid_t child_pid[3], check[3];
    int des1[3][2], status, bytes_read = 0, stat_loc[3], i, j, k;
    char readbuf[1024], charLine[80] = "-----CMD  : --------------------------------------------------------------------";  
    char *file = (char*)malloc(sizeof(char)*len);
    
    for(i = 0; i < 3; i++) {
        args[i] = get_command(i);
        if(pipe(des1[i]) == -1) {
            perror("Pipe failed");
            exit(1);
        }
    }

    printf("file>");
    readline = getline(&file, &write, stdin);
    
    child_pid[0] = fork();
   
    if (child_pid[0] == 0) {
        
        close(STDOUT_FILENO);
        dup(des1[0][1]);
        close(des1[0][0]);
        close(des1[0][1]);
    
        gettimeofday(&start[0], NULL);
    
        execvp(args[0][0], args[0]);
        printf("execvp not successful\n");
    } else {
        
        child_pid[1] = fork();

        if (child_pid[1] == 0) {
        
            close(STDOUT_FILENO);
            dup(des1[1][1]);
            close(des1[1][0]);
            close(des1[1][1]);

            gettimeofday(&start[1], NULL);

            execvp(args[1][0], args[1]);
            printf("execvp not successful\n");
        } else {            
            
            child_pid[2] = fork();
            
            if (child_pid[2] == 0) {
        
                close(STDOUT_FILENO);
                dup(des1[2][1]);
                close(des1[2][0]);
                close(des1[2][1]);

                gettimeofday(&start[2], NULL);

                execvp(args[2][0], args[2]);
                
                printf("execvp not successful\n");
            } else {
                check[2] = waitpid(child_pid[2], &stat_loc[2], WUNTRACED);
                gettimeofday(&stop[2], NULL);
                printf("Third process finished...\n");            
            }
            check[1] = waitpid(child_pid[1], &stat_loc[1], WUNTRACED);
            gettimeofday(&stop[1], NULL);
            printf("Second process finished...\n");            
        }
        check[0] = waitpid(child_pid[0], &stat_loc[0], WUNTRACED);
        gettimeofday(&stop[0], NULL);
        printf("First process finished...\n");            
    }
    
    for(i = 0; i < 3; i++) {
        j = 0;
        k = 12;
        printf("-----CMD %d: ", (i + 1));
        while (args[i][j] != NULL) {
            printf("%s", args[i][j]);
            k += strlen(args[i][j]);
            j++;
        }
        char * p = charLine + k;
        printf("%s\n", p);
        if(check[i] == child_pid[i]) {
            close(des1[i][1]);
            dup(des1[i][0]);

            do {
                bytes_read = read(des1[i][0], readbuf, sizeof(readbuf));
                readbuf[bytes_read] = '\0';
                printf("%.*s",bytes_read, readbuf);
            } while (bytes_read > 0);
        }
        int l = (int)(((stop[i].tv_sec)*1000 + (stop[i].tv_usec/1000)) - ((start[i].tv_sec*1000) + (start[i].tv_usec/1000)));
        printf("Result took: %dms\n",  l);
    }
    
    free(args);
    free(file);
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
