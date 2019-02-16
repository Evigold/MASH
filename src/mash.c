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
    int stat_loc;
    pid_t child_pid;
}