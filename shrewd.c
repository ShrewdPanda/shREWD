#include <sys/types.h>
#include <sys/wait.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFSIZE 1024
#define TRUE 1
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define TOKEN_DELIM "\t\r\n\a "
#define TOKEN_SIZE 64


/* prototypes for each of the calls */
int call_cd(char **args);
int call_exit(char **args);
int call_help(char **args);
char *get_userpath();

/* list of calls */
char *command_list[] = {
	"cd",
	"exit",
	"help"
};

int (*command_calls[]) (char **) = {
	&call_cd,
	&call_exit,
	&call_help
};

int num_commands()
{
	return sizeof(command_list) / sizeof(char *);
}

/* explicit declarations for shell function calls */
int call_cd(char **args)
{
	if (args[1] == NULL)
	{
		chdir(getenv("HOME"));
	}
	else
	{
		if (chdir(args[1]) != 0)
			perror("shrewd");
	};
	return 1;
}


int call_exit(char **args)
{
	exit(0);
}

int call_help(char **args)
{
	printf("\n\nshREWD :: developed by Brian Erickson\n");
	printf("Copyright (c) 2019, MIT License\n\n");
	printf("Permission is hereby granted, free of charge, to any person obtaining a copy\n");
	printf("of this software and associated documentation files (the 'Software'), to deal\n");
	printf("in the Software without restriction, includeing without limitation the right\n");
	printf("to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n");
	printf("copies of the Software, and to permit persons to whom the Software is\n");
	printf("furnished to do so, subject to the following conditions:\n\n");
	printf("The above copyright notice and this permission notice shall be included in all\n");
	printf("copies of substantial portions of the Software\n\n");
	printf("THE SOFTWARE IS PROVIDED 'AS IS' WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n");
	printf("IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTIABILITY,\n");
	printf("FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n");
	printf("AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n");
	printf("LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n");
	printf("OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE\n");
	printf("SOFTWARE\n\n");
	printf("Simple shell scripting interface\n");
	printf("::::::::::::::::::::::::::::::::::\n\n");
	
	for (int i = 0; i < num_commands(); i++)
		printf("\t%s\n", command_list[i]);
	printf("\n\n");
	return 1;
}

char *readline()
{
	/* initial memory allocation */
	char *buf = malloc(sizeof(char) * BUFSIZE);

	/* handle initial allocation exceptions */
	if (!buf)
	{
		fprintf(stderr, "shrewd: memory allocation error\n");	
	}

	/* declare variables (will be altered in while() loop) */
	int c;
	int position = 0;
	int bufsize = BUFSIZE;

	/* store data in buffer */
	while (TRUE)
	{
		c = getchar();

		if (c == EOF)
		{
			exit(EXIT_SUCCESS);
		}
		/* re-encode newline chars as null-termination, return the buffer */
		else if (c == '\n')
		{
			buf[position] = '\0';
			return buf;
		}
		/* normal behavior - store values in buffer */
		else
		{
			buf[position] = c;
		}
		position++;

		/* if data exceeds size of buffer */
		if (position >= bufsize)
		{
			/* increase the buffer size by fixed value */
			bufsize += BUFSIZE;
			/* increase the memory allocation by that fixed value */
			buf = realloc(buf, bufsize);
		
			/* handle mem-alloc exceptions */
			if (!buf)
			{
				fprintf(stderr, "shrewd: memory re-allocation error\n");
				exit(EXIT_FAILURE);
			}
		}
	}
}

char **parse(char *line)
{
	int bufsize = TOKEN_SIZE, position = 0;
	char *token, **tokens_backup;
	
	/* allocate memory for tokens */
	char **tokens = malloc(sizeof(char*) * bufsize);
	/* handle allocation exceptions */
	if (!tokens)
	{
		fprintf(stderr, "shrewd: memory allocation error\n");
		exit(EXIT_FAILURE);
	}
	
	/* tokenize the line */
	token = strtok(line, TOKEN_DELIM);

	while (token != NULL)
	{
		/* store each token in token[position] */
		tokens[position] = token;
		position++;
		
		/* if input is too big for buffer */
		if (position >= bufsize)
		{
			/* increase buffer size */
			bufsize += TOKEN_SIZE;
			/* store a backup */
			tokens_backup = tokens;
			/* increase the memory allocation */
			tokens = realloc(tokens, sizeof(char*) * bufsize);
			
			/* handle memory allocation exceptions */
			if (!tokens)
			{
				free(tokens_backup);
				fprintf(stderr, "shrewd: memory allocation error.\n");
				exit(EXIT_FAILURE);
			}
		}
		
		/* token array is cleared*/
		token = strtok(NULL, TOKEN_DELIM);
	}
	/* last position is set as null */
	tokens[position] = NULL;
	return tokens;
}

int dispatch(char **args)
{
	pid_t pid;
	int status;
	
	/* start new process */
	pid = fork();

	/* if fork() is successful */
	if (pid == 0)
	{
		/* execute arguments; handle exception of argument execution failure */
		if (execvp(args[0], args) == -1)
			perror("shrewd");
		/* exit fail regardless of success executing */
		exit(EXIT_FAILURE);
	}
	/* if there is execution exception */
	else if (pid < 0) 
	{
		perror("shrewd");
	}
	/* if there is timeout error */
	else
	{
		do
		{
			/* suspend execution, wait for process state to change */
			/* return if child has stopped */
			waitpid(pid, &status, WUNTRACED);
			/* continue while the process hasn't exited or raised a signal */
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}
	return 1;
}

int execute(char **args)
{
	/* exception: if no input - empty string */
	if (args[0] == NULL)
		return 1;
	
	/* rule: search list of commands to find match with input string */
	for (int i = 0; i < num_commands(); i++)
	{
		/* if match is found, return command and execute */
		if (strncmp(args[0], command_list[i], BUFSIZE) == 0)
			return(*command_calls[i])(args);
	}

	return dispatch(args);
}

void init()
{
	char *lines;
	char **args;
	int status;
	char cwd[1024];

	system("clear");

	do
	{	
		printf("%s @ %s\n:: ", getenv("USER"), getcwd(cwd, sizeof(cwd)));
		lines = readline();
		args = parse(lines);
		status = execute(args);
	
		/* free DMA after use to avoid memleaks */	
		free(lines);
		free(args);

	} while(status);
}

int main(int argc, char **argv)
{
	init();
	return EXIT_SUCCESS;
}
