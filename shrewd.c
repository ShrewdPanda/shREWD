#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 1024
#define TRUE 1
#define EXIT_FAILURE 1
#define EXIT_SUCCESS 0
#define TOKEN_DELIM "\t\r\n\a"
#define TOKEN_SIZE 64


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
			/* double the buffer size */
			bufsize += BUFSIZE;
			/* double the memory allocation */
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
		tokens[position] = token;
		position++;
		
		/* if input is too big for buffer */
		if (position >= bufsize)
		{
			/* double the buffer size */
			bufsize += TOKEN_SIZE;
			/* store a backup */
			tokens_backup = tokens;
			/* double the memory allocation */
			tokens = realloc(tokens, sizeof(char*) * bufsize);
			
			/* handle memory allocation exceptions */
			if (!tokens)
			{
				free(tokens_backup);
				fprintf(stderr, "shrewd: memory allocation error.\n");
				exit(EXIT_FAILURE);
			}
		}
		
		/* set value after last as NULL */
		token = strtok(NULL, TOKEN_DELIM);
	}
	/* set value after last as NULL (if any were valid) and return */
	tokens[position] = NULL;
	return tokens;
}

int execute(char **args)
{

}

void shrewd_loop()
{
	char *lines;
	char **args;
	int status;

	system("clear");

	do
	{	
		printf("shrewd> ");
		lines = readline();
		args = parse(lines);
		//status = execute(args);
		free(lines);

	} while(TRUE);
}

int main(int argc, char **argv[])
{
	shrewd_loop();
	return EXIT_SUCCESS;
}
