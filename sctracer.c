#include <sys/ptrace.h>
#include <sys/reg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

int main(int argc, char *argv[])
{
	pid_t child = fork();

	int numPossibleSysCalls = 338;

	int count[numPossibleSysCalls];

	for (int z = 0; z < numPossibleSysCalls; z++)
		count[z] = 0;

	int argCount = 0;
	char usefulArgs[10000] = {0};
	char *s = argv[1];

	//copy argv[] to holder string
	strncpy(usefulArgs, argv[1], strlen(s));

	//start splitting string with strtok, consuming whitespace
	char *splitter;
	splitter = strtok(usefulArgs, " ");

	char *builtArgs[100];

	//delineate all of argv[1] into builtArgs
	while (splitter != NULL)
	{
		builtArgs[argCount] = splitter;
		argCount++;
		splitter = strtok (NULL, " ");
	}

	FILE *file;

	//open FILE with correct name
	file = fopen(argv[argc-1], "w");

	//start tracing...
	if (child == 0)
	{
		ptrace(PTRACE_TRACEME);

		raise(SIGSTOP);

		execvp(builtArgs[0], builtArgs);
	}
	else
	{
		int status, syscall_num;

		waitpid(child, &status, 0);

		ptrace(PTRACE_SETOPTIONS, child, 0, PTRACE_O_TRACESYSGOOD);

		do
		{
			ptrace(PTRACE_SYSCALL, child, 0, 0);
			waitpid(child, &status, 0);

			//catch and count sys calls
			if ((WIFSTOPPED(status) || WSTOPSIG(status) & 0x80))
			{
				syscall_num = ptrace(PTRACE_PEEKUSER, child, sizeof(long)*ORIG_RAX, NULL);

				count[syscall_num]++;
			}
		}
		while (!(WIFEXITED(status) || (WIFSIGNALED(status) & 0x80)));

		//account for "double" values since call and return counted;
		//don't lose single calls
		for (int i = 0; i < numPossibleSysCalls; i++)
		{
			if (count[i] != 0)
			{
				if ((count[i] % 2) == 1)
					count[i] = (count[i] / 2) + 1;
				else
					count[i] = (count[i] / 2);
			}
		}

		//printer
		for (int i = 0; i < numPossibleSysCalls; i++)
		{
			if (count[i] != 0)
				fprintf(file, "%i	%i\n", i, count[i]);
		}
	}

}


