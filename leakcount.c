#define _GNU_SOURCE

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>

int main(int argc, char *argv[])
{
	if (fork() == 0)
	{
		char *usefulArgs[argc];

		//prep environment vars
		char *envs[] = {"LD_PRELOAD=./memory_shim.so", NULL};

		//parse for cmd line arguments other than the first
		for (int i = 1; i < argc; i++)
		{
			usefulArgs[i - 1] = argv[i];
		}

		//NULL terminate argument string
		usefulArgs[argc - 1] = NULL;

		int j = execvpe(usefulArgs[0], usefulArgs, envs);

		if (j < 0)
			perror("execvpe");
	}

	wait(NULL);
}
