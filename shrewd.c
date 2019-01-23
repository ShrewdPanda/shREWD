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
#define TOKEN_DELIM "\t\r\n\a"
#define TOKEN_SIZE 64


/* prototypes for each of the calls */
int call_exit(char **args);
int call_help(char **args);

/* list of calls */
char *command_list[] = {
	"exit",
	"help"
};

int (*command_calls[]) (char **) = {
	&call_exit,
	&call_help
};

int num_commands()
{
	return sizeof(command_list) / sizeof(char *);
}

/* explicit declarations for shell function calls */
int call_exit(char **args)
{
	return 0;
}

int call_help(char **args)
{
	printf("shREWD :: developed by Brian Erickson\n");
	printf("Simple shell scripting interface\n");
	printf("::::::::::::::::::::::::::::::::::\n\n");
	
	for (int i = 0; i < num_commands(); i++)
		printf(" %s\n", command_list[i]);
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
		/* reset buf position, return buf if 'ENTER' */
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
			/* increase the buffer size */
			bufsize += BUFSIZE;
			/* increase the memory allocation */
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
	/* last position is cleared */
	tokens[position] = NULL;
	return tokens;
}

int dispatch(char **args)
{
	pid_t pid;
	int status;

	pid = fork();

	if (pid == 0)
	{
		if (execvp(args[0], args) == -1)
			perror("shrewd");
		exit(EXIT_FAILURE);
	}
	else if (pid < 0) 
	{
		perror("shrewd");
	}
	else
	{
		do
		{
			waitpid(pid, &status, WUNTRACED);
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

	system("clear");

	do
	{	
		printf("shrewd :: ");
		lines = readline();
		args = parse(lines);
		status = execute(args);
		free(lines);
		free(args);

	} while(TRUE);
}

int main(int argc, char **argv[])
{
	init();
	return EXIT_SUCCESS;
}
