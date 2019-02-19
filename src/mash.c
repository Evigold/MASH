#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

int exe();

char **get_command(int i);

int cd(char *);

void print_finished(int);

void append_file(char *, char ***);

long get_time(struct timeval *, struct timeval *);

int main(void) {
    exe();
    return 0;
    
}

int exe() {
    struct timeval start[3], stop[3];
    char *token;
    size_t len, write;
    ssize_t readline = 0;
    char ***args = (char***)malloc(sizeof(char**)*3);;
    pid_t child_pid[3], check[3];
    int des1[3][2], status, bytes_read = 0, stat_loc[3], i, j, k;
    char readbuf[1024], charLine[80] = "-----CMD  : --------------------------------------------------------------------";  
    char *file = (char*)malloc(sizeof(char)*len);
    int count = 0;
    long totalTime = 0;
    int file_handler = 0;

    struct timeval startTime;
    struct timeval endTime;

        
    for(i = 0; i < 3; i++) {
        //Populate list of flags for command.
        args[i] = get_command(i);
        if(pipe(des1[i]) == -1) {
            perror("Pipe failed");
            exit(1);
        }
    }

    const char *sep = "\n";
    printf("file>");
    readline = getline(&file, &write, stdin);
    //Tokenize and remove newline char from filename input.
	token = strtok(file, sep);
    if (token != NULL) {
        file = strdup(token);
    }

    //Append the filename to the list of arguments of each command.
    append_file(&file[0], args);
    printf("%s\n", &file[0]);

    
    //Begin forking.
    child_pid[0] = fork();
    // Start timer for all executions
    gettimeofday(&startTime, NULL);

    //Run process in first fork.
    gettimeofday(&start[0], NULL);

    if (child_pid[0] < 0 ) {
        printf("CMD1:[SHELL 1] STATUS CODE=%d\n", -1);
    } else if (child_pid[0] == 0) {
        
        close(STDOUT_FILENO);
        dup(des1[0][1]);
        close(des1[0][0]);
        close(des1[0][1]);
    
        execvp(args[0][0], args[0]);
        printf("CMD1:[SHELL 1] STATUS CODE=%d\n", -1);
    } else {
        
        child_pid[1] = fork();
        //Run process of 2nd fork.
        gettimeofday(&start[1], NULL);
        
        if (child_pid[1] < 0 ) {
            printf("CMD2:[SHELL 2] STATUS CODE=%d\n", -1);
        } else if (child_pid[1] == 0) {
        
            close(STDOUT_FILENO);
            dup(des1[1][1]);
            close(des1[1][0]);
            close(des1[1][1]);

            execvp(args[1][0], args[1]);
            printf("CMD2:[SHELL 2] STATUS CODE=%d\n", -1);
        } else {            
            
            child_pid[2] = fork();
            //Run process of 3rd fork.

            gettimeofday(&start[2], NULL);
            if (child_pid[0] < 0 ) {
                printf("CMD3:[SHELL 3] STATUS CODE=%d\n", -1);
            } else if (child_pid[2] == 0) {

                close(STDOUT_FILENO);
                dup(des1[2][1]);
                close(des1[2][0]);
                close(des1[2][1]);

                execvp(args[2][0], args[2]);
                printf("CMD3:[SHELL 3] STATUS CODE=%d\n", -1);
            } else {
                check[2] = waitpid(child_pid[2], &stat_loc[2], WUNTRACED);
                gettimeofday(&stop[2], NULL);
                count++;
                if (check[2] == child_pid[2]) {
                    print_finished(count);    
                } 
            }
            check[1] = waitpid(child_pid[1], &stat_loc[1], WUNTRACED);
            gettimeofday(&stop[1], NULL); 
            count++;
            if (check[1] == child_pid[1]) {
                print_finished(count);    
            }       
        }
        check[0] = waitpid(child_pid[0], &stat_loc[0], WUNTRACED);
        gettimeofday(&stop[0], NULL);
        count++;
        if (check[0] == child_pid[0]) {
            print_finished(count);    
        } 
    }
    if (count == 3) {
        gettimeofday(&endTime, NULL);
        for(i = 0; i < 3; i++) {
            j = 0;
            k = 12;
            printf("-----CMD %d: ", (i + 1));
            while (args[i][j] != NULL) {
                if (strcmp(args[i][j], file) != 0) {
                    printf("%s ", args[i][j]);
                    k += strlen(args[i][j]) + 1;
                }
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

            long l = get_time(&start[i], &stop[i]);
            totalTime += l;
            printf("Result took: %ld ms\n",  l);
        }

        for (i = 0; i < 80; i++) {
            printf("-");
        }
        printf("\nChildren process IDs: ");
        for (i = 0; i < 3; i++) {
            printf("%d", child_pid[i]);
            if (i < 2) {
                printf(" ");
            } else {
                printf(".\n");
            }
        }

        totalTime = get_time(&startTime, &endTime);
        printf("Total elapsed time:%ldms\n", totalTime);
    }
 
    free(args);
    free(file);
    return 0;
}

long get_time(struct timeval * start, struct timeval * stop) {
    long elapsedTime = (stop->tv_sec * 1000 + stop->tv_usec / 1000) 
                        - (start->tv_sec * 1000 + start->tv_usec / 1000);
    return elapsedTime;
}

void append_file(char * file, char ***args) {
    int i = 0;
    for (i = 0; i < 3; i++) {
        int j = 0;
        while (j >= 0) {
            if (args[i][j] == NULL) {
                args[i][j] = file;
                args[i][j + 1] = NULL;
                j = -1;
            } else {
                j++;
            }
        }
    }
}

void print_finished(int c) {
    if (c == 1) {
        printf("First process finished...\n");
    } else if (c == 2) {
        printf("Second process finished...\n");   
    } else if (c == 3) {
         printf("Third process finished...\n");  
    }
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
