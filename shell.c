#include "commandUtil.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	while (1) {
		char comando[80];
		printf("$ ");
		fgets(comando, 1024, stdin);
		char *modifiedCommand = removeExtraSpaces(comando);
		int pfd_1[2];

		if (!strcmp("exit", modifiedCommand)) {
			break;
		}

		char **args1 = malloc(64 * sizeof(char *));
		char **args2 = malloc(64 * sizeof(char *));

		separarArgumentos(args1, modifiedCommand);

		int piper = checkpip(args1, args2);

		int j = 0;

		while (1) {
			if (args2[j] == NULL) {
				break;
			}
			j++;
		}

		pid_t pid = fork();

		if (!pid) {
			if (!piper) {
				ejecutr(args1);
			} else {
				ejecutaPipe(args1, args2);
			}
		} else {
			wait(NULL);
    }
	}
	return 0;
}
