/*************************************************************************/
/******************Simple Shell for Simple People*************************/
/*************************************************************************/


#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>


#define INPUT_CMD       0
#define INPUT_VAR       1

extern char **environ;

char local_var_name[100][20];
char local_var_value[100][20];
int local_var_count = 0;

/*
 * function to split the input buffer into tokens using space ' ' and equal '=' delimiters
 * returns the tokens in arrays of strings, shall be freed after usage
 * the function also returns the status which is INPUT_VAR if equal '=' 
 * is found inside the buffer, otherwise it returns INPUT_CMD
 */
char **simple_tokenizer(char *buf, int buf_len, int *status)
{
	int str_count = 0;
	int char_count = 0;
	int cont_spaces_flag = 1;
	
	*status = INPUT_CMD;

	char **buf_parsed;
	buf_parsed = malloc(10 * sizeof(char *));
	buf_parsed[str_count] = malloc(100 * sizeof(char));

	for (int i = 0; i < buf_len; i++) {
		if (buf[i] == '=' || buf[i] == ' ') {
			if (cont_spaces_flag == 0) {
				buf_parsed[str_count++][char_count] = '\0';
				buf_parsed[str_count] = malloc(100 * sizeof(char));
				char_count = 0;
				cont_spaces_flag = 1;
			}		
			if (buf[i] == '=')
			{
				//for setting local variables, no spaces shall be detetced before or after the equal sign
				//so check if spaces detetcted before or after the equal sign, do not consider it as INPUT_VAR
				if((i > 0 && buf[i-1] == ' ') || (i < buf_len - 1 && buf[i+1] == ' '))
					*status = INPUT_CMD;
				else
					*status = INPUT_VAR;
			}	
		} else {
			buf_parsed[str_count][char_count] = buf[i];
			char_count++;
			cont_spaces_flag = 0;
		}
	}

	if (cont_spaces_flag == 0) {
		// assign the null charactor for the last word
		buf_parsed[str_count++][char_count++] = '\0';
	}
	// assign extra word with null
	buf_parsed[str_count] = NULL;	

	return buf_parsed;
}

/*
 * function to search for input variable across the local variables in local_var_name[]
 * if found return index of the local variable, otherwise return -1
 */
int search_across_local_var(char *var)
{
	int index = -1;

	for (int i = 0; i < local_var_count; i++)
	{
		if(!strcmp(local_var_name[i], var))
		{
			index = i;
		}
	}
	
	return index;
}

int main()
{
	int status, input_status;
	char buf[256];
	int buf_len;
	char exitCmd[] = "exit";
	char setCmd[] = "set";
	char exportCmd[] = "export";
	int argc = 0;
	char **argv;
		
	char hostname[20] = "pc";
	gethostname(hostname, 20);	
	char pwd[50] = "~";
	getcwd(pwd, 50);
	
	char username[20] = "user";
	for(int i = 0; environ[i] != NULL; i++)
	{
		if(!strncmp("USERNAME=", environ[i], 9))
			strcpy(username, &environ[i][9]);
	}

	while (1) {
		//printing the default prompt for the user
		printf("%s@%s:%s$ ", username, hostname, pwd);
		//getting the input commands
		fgets(buf, 256, stdin);

		buf_len = strlen(buf) - 1;

		//detecting if only enter is pressed besides clipping the extra enter '\n' character
		if (buf_len == 0)
			continue;
		else if (buf[buf_len] == '\n')
			buf[buf_len] = '\0';

		//end the program if "exit" command is detected
		if (!strcmp(exitCmd, buf)) {
			break;
		}
		//print the availavble local variable if "set" command is detected
		else if (!strcmp(setCmd, buf)) {
			for (int i = 0; i < local_var_count; i++) {
				printf("local variable[%d]: %s = %s\n", i, local_var_name[i], local_var_value[i]);
			}
			continue;
		}

		//split the input stream into tokens
		argv = simple_tokenizer(buf, buf_len, &input_status);

		//check if "export" command is detected, then set the variable if found to the environment variables
		if (!strcmp(exportCmd, argv[0])) {
			
			if(argv[1] == NULL)
			{
				fprintf(stderr, "error: too few arguments!!\n");
			}
			else
			{
				int index = search_across_local_var(argv[1]);
				if(index == -1)
				{
					fprintf(stderr, "error: variable %s is not found!!\n", argv[1]);
				}
				else
				{
					setenv(local_var_name[index], local_var_value[index], 1);
				}
			}				
		}
		//check if input local variable is requested to be added to the database
		//if found overwrite it with the new value, otherwise create new entry
		else if (input_status == INPUT_VAR) {
			if(argv[1] == NULL)
			{
				fprintf(stderr, "error: too few arguments!!\n");
			}
			else
			{
				int index = search_across_local_var(argv[0]);		
				if(index == -1)
				{
					//new variable
					strcpy(local_var_name[local_var_count], argv[0]);
					strcpy(local_var_value[local_var_count], argv[1]);
					local_var_count++;
				}
				else
				{
					//overwrite variable
					strcpy(local_var_value[index], argv[1]);
				}
			}
		}
		//if the entered stream is command, fork is used to create child process
		//then check is done if valid command, then execute it in the child process
		else if (input_status == INPUT_CMD) {
			int pid = fork();
			if (pid == -1) {
				fprintf(stderr, "error: fork error number %d!!\n", errno);
			} else if (pid == 0) {
				// Child process

				// removed as "KILLED" is printed when using kill(2)
				// if (!strcmp(exitWord, buf)) {
				// kill(0, SIGKILL);
				// }
				
				for(int i = 0; argv[i] != NULL; i++)
				{
					if(!strcmp("<", argv[i]))
					{
						//printf("input redirection\n");
						if(argv[i+1] == NULL)
						{
							fprintf(stderr, "error: no input file is defined!!\n");
							goto funcExit;
						}
						
						int inFD  = open(argv[i+1], O_RDONLY);
						
						if(inFD == -1)
						{
							switch(errno)
							{
								case ENOENT:
									fprintf(stderr, "error: file \"%s\" does not exist!!\n", argv[i+1]);
									break;
								default:
									fprintf(stderr, "error number %d occured in opening input file\n", errno);
									break;
							}

							goto funcExit;
						}
						
						int inRet = dup2(inFD, 0 /*input stream*/);
						close(inFD);
						
						argv[i++] = NULL;
						argv[i] = NULL;
					}					
					else if(!strcmp(">", argv[i]))
					{
						//printf("output redirection\n");
						if(argv[i+1] == NULL)
						{
							fprintf(stderr, "error: no output file is defined!!\n");
							goto funcExit;
						}
						
						int outFD  = open(argv[i+1], O_CREAT|O_WRONLY|O_TRUNC, 0644);
						
						if(outFD == -1)
						{
							fprintf(stderr, "error number %d occured in opening or creating output file\n", errno);
							goto funcExit;
						}
						
						int outRet = dup2(outFD, 1 /*output stream*/);
						close(outFD);
						
						argv[i++] = NULL;
						argv[i] = NULL;
					}					
					else if(!strcmp("2>", argv[i]))
					{
						//printf("error redirection\n");			
						if(argv[i+1] == NULL)
						{
							fprintf(stderr, "error: no error file is defined\n");
							goto funcExit;
						}
						
						int errFD  = open(argv[i+1], O_CREAT|O_WRONLY|O_TRUNC, 0644);
						
						if(errFD == -1)
						{
							fprintf(stderr, "error number %d occured in opening or creating error file\n", errno);
							goto funcExit;
						}
						
						int errRet = dup2(errFD, 2 /*error stream*/);
						close(errFD);
						
						argv[i++] = NULL;
						argv[i] = NULL;
					}
					else
					{
						//Do Nothing
					}
				}				
				
				// execve(buf, argv, environ);
				execvp(argv[0], argv);

				//failure case of execve
				switch (errno) {
				case ENOENT:
					fprintf(stderr, "error: command not found!!\n");
					break;
				default:
					fprintf(stderr, "error: execve error number %d!!\n", errno);
					break;
				}
				
			funcExit:
				//terminate the child process
				_exit(EXIT_FAILURE);
				
			} else {
				// Parent process
				wait(&status);
			}
		}
		
		free(argv);
	}

	return 0;
}
