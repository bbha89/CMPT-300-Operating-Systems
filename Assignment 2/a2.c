#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //fork(), exec(), pipe..
#include <sys/wait.h> //wait()
#include <sys/types.h> //pid_t: process IDs
#include <time.h>

#define MAX 20

struct Commands{ //type struct Commands
	char *name;
	int code;
	time_t raw;
};

struct Variables{
	char *name;
	char *value;
};

struct Commands Command[MAX];
struct Variables Variable[MAX];

void scriptMode(char buff[], int file_line) {
	FILE *file;
	int count = 0;
	int found = 0;
	file = fopen("test", "r");

	char a[200];
	while(fgets(a, 200, file) != NULL) {
		if (count == file_line && found == 0) {
			strcpy(buff, a);
			found = 1;	
		} else {
			count++;
		}
	}
	if (found == 0) { // All lines visited.
		exit(0);
	} 
	fclose(file);
}

int checkTheme (char *arr[], int cPos, int invalidCheck) {
	if (!strcmp(arr[1], "yellow")) {
		printf("\e[1;33m"); //yellow
		return 0;
	} else if (!strcmp(arr[1], "green")) {
		printf("\e[0;32m"); //green	
		return 0;		
	} else {
		if (invalidCheck == 1) { //script mode invalid cmd
			printf("Missing keyword or command, or permission problem in Script Mode. Bye!\n");
			exit(0);
		}
		printf("Failed to change theme colour\n");
		return 1;	
	}
}

int checkLog(char *arr[], int cPos) {
	for (int i = 0; i < cPos; ++i)
	{
		printf("%s", asctime(localtime(&Command[i].raw)));//& to get address of said value 
		printf("%s %d\n", Command[i].name, Command[i].code);
	}
	return 0;
}
void history(char *currName, int currCode, int structPos) {
	time_t rawtime;
	Command[structPos].name = currName;
	Command[structPos].code = currCode;	
	Command[structPos].raw = time(&rawtime);
}

void createVar(char *tkn, char *val, int vPos, int var_state) {
	if (var_state != 1) { 			// no repeated variable
		Variable[vPos].name = tkn;
		Variable[vPos].value = val;	
	} else if (var_state == 1) { 	// repeated variable
			Variable[vPos].value = val;
		}
	}

int checkDollar (char *arr[], char*var_dollar, int cPos, int vPos, int invalidCheck) {
	char *var = strdup(arr[0]);
	char *val = strdup(arr[0]);
	char *logCmd = strdup(arr[0]);
	char *tkn = strtok(var, "="); 	// contains var
	val = strchr(val, '=');			// contains val
	tkn++;	//remove "$"
	// if (val_check >=1)
	// {
	// 	val++; 	//remove "="
	// }

	if (val != NULL && tkn != NULL ) {
		val++;
		if (val == NULL) {
			if (invalidCheck == 1) { //script mode invalid cmd
				printf("Missing keyword or command, or permission problem in Script Mode. Bye!\n");
				exit(0);
			}
			printf("No variable/value was typed with $.\n");
			history(logCmd, 1, cPos);	
			} else {
			int var_size = strlen(tkn); // len of var
			int val_size = strlen(val);	// len of val

			// *has variable & has value*
			if (var_size > 0 && val_size > 0) {
				int var_state = 0;
				int var_pos = 0;
				// Check if variable exists
				for (int i = 0; i < vPos; ++i) {
					if (strcmp(Variable[i].name, tkn) == 0) {
						var_state = 1;
						var_pos = i;
					}
				}
				if (var_state == 1) {	// variable exist
					createVar(tkn, val, var_pos, var_state);
					history(logCmd, 0, cPos);
				} else {				// new variable
					createVar(tkn, val, vPos, var_state);
					history(logCmd, 0, cPos);
					return 0;
				}
			} else {
				if (invalidCheck == 1) { //script mode invalid cmd
					printf("Missing keyword or command, or permission problem in Script Mode. Bye!\n");
					exit(0);
				}
				printf("No variable/value was typed with $.\n");
				history(logCmd, 1, cPos);
			}
		}
	} else {
		if (invalidCheck == 1) { //script mode invalid cmd
			printf("Missing keyword or command, or permission problem in Script Mode. Bye!\n");
			exit(0);
		}
		printf("No variable/value was typed with $.\n");
		history(logCmd, 1, cPos);
	}
	return 1;
}

void print_cmd(char *arr[], int cPos, int pos, int vPos, int invalidCheck) {

	char *logCmd = strdup(arr[0]);
	// no arguments
	if (arr[1] == NULL) {
		printf("no arguments found. empty print\n");
		history(logCmd, 0, cPos);
	} 
	// arguments found
	else {
		// check for invalid arguments
		int checkArr[vPos+1]; // +1 for 2nd.. arguments
		for (int i = 1; i < pos; ++i) {
			char first_letter[80];
			char *curArg = strdup(arr[i]);
			// check first letter for each argument containing $
			strncpy(first_letter, curArg, 1);
			first_letter[1] = '\0'; // add null char to prevent garbage
			
			// check if variable exist for each argument
			// 0: string, 1: $<VAR> exist, 2: not exist
			if (strcmp(first_letter, "$") == 0) {
				int check = 0;
				curArg++;			// remove the $
				if (vPos == 0) {	// no stored variables to check
					checkArr[i] = 2;
				} else {
					for (int j = 0; j < vPos; ++j) {
						// var exist and is printable
						if (strcmp(curArg, Variable[j].name) == 0) {
							checkArr[i] = 1; // set to true
							check = 1;
						} 
						// var does not exist
						else if (strcmp(curArg, Variable[j].name) != 0 && check == 0) {
							checkArr[i] = 2;
						}
					}
				}
			} 
			// string argument
			else {
				checkArr[i] = 0;
			}
		}

		int finalCheck = 0;
		for (int i = 1; i < pos; ++i)
		{
			if (checkArr[i] == 2) {
				finalCheck = 1;
			}
			// printf("this is array: %d\n", checkArr[i]);
			// printf("this is finalcheck: %d\n", finalCheck);
		}
		if (finalCheck == 1) {
			if (invalidCheck == 1) { //script mode invalid cmd
				printf("Missing keyword or command, or permission problem in Script Mode. Bye!\n");
				exit(0);
			}
			printf("A variable does not exist. Invalid print.\n");
			history(logCmd, 1, cPos);
		} 
		// all arguments are good to print
		else {
			// printf("%d\n", checkArr[0]);
			for (int i = 1; i < pos; ++i)
			{
				// string argument
				if (checkArr[i] == 0) {
					printf("%s ", arr[i]);
					history(logCmd, 0, cPos);
				} 
				// variable argument
				else if (checkArr[i] == 1) {
					char *print_dollar = strdup(arr[i]); // 2nd arguement
					char print_var[80];

					strncpy(print_var, print_dollar, 1); // $
					print_var[1] = '\0'; // add null char to prevent garbage

					//print variable
					if (strcmp(print_var, "$") == 0) {
						print_dollar++;		// remove the $
						// int var_exist = 0;
						for (int i = 0; i < vPos; ++i) {
							if (strcmp(print_dollar, Variable[i].name) == 0) {
								printf("%s ", Variable[i].value);
								// var_exist = 1;
								history(logCmd, 0, cPos);
							}
						}
					} 	
				}
			}
			printf("\n");
		}

	}
}

int main(int argc, char *argv[])
{
	pid_t pid_child;
	char buff[80];
	char *arr[80];
	int cPos = 0;
	// shell_pid = getpid();
	int vPos = 0;
	int pos;
	int invalidCheck = 0;

	int fds[2];

	int file_line = 0;
	if (argc > 1 && !strcmp(argv[1], "test")) {
		printf("Testing the script mode\n");
	} 
	while(1) {
	 	//Script mode 
		if (argc > 1 && !strcmp(argv[1], "test")) {
			scriptMode(buff, file_line);
			file_line++;
			invalidCheck = 1;
		} 
	 	//Interactive mode
		else {
			printf("cshell:$ ");
			if (fgets(buff, 80, stdin) == NULL) {
				return EXIT_SUCCESS;
			}
		}
		// printf("this is 1: %d\n", invalidCheck);
		//Break line into words
		char * token = strtok(buff, " \t\n"); 	//delimiter: " ", tabs and \n
		pos = 0;
		while (token != NULL) {
			arr[pos++] = token; 				//assigns and increments i
			token = strtok(NULL, " \t\n");
		}
		arr[pos] = '\0'; // null terminated (end of array)

		// Check if a command is entered
		if (arr[0] != NULL) {
			char *logCmd = strdup(arr[0]);
			char *var_dollar = strdup(arr[0]); 	// check arr[0] starts w/ $
			char save_dollar[40];
			strncpy(save_dollar, var_dollar,1); // store first letter of arr[0]
			save_dollar[1] = '\0';

			if (!strcmp(arr[0], "exit")) {				// exit
				exit(0);
			} else if (!strcmp(arr[0], "theme")) {		// theme
				int theme = checkTheme(arr, cPos, invalidCheck);
				if (theme == 0) {
					history(logCmd, 0, cPos);
				} else {
					if (invalidCheck == 1) { //script mode invalid cmd
						printf("Missing keyword or command, or permission problem in Script Mode. Bye!\n");
						exit(0);
					}
					history(logCmd, 1, cPos);
				}
				cPos++;
			} else if (!strcmp(arr[0], "log")) {		// log
				int log = checkLog(arr, cPos);
				if (log == 0) {
					history(logCmd, 0, cPos);
				}
				cPos++;
			} else if (!strcmp(arr[0], "print")) {
				print_cmd(arr, cPos, pos, vPos, invalidCheck);
				cPos++;
			} 
			// else if (strcmp(arr[0], "check") == 0) { // testing purposes
			// 	for (int i = 0; i < vPos; ++i)
			// 	{
			// 		printf("pos: %d name: %s\n", i, Variable[i].name);
			// 		printf("pos: %d value: %s\n", i, Variable[i].value);
			// 	}
			// } 
			else if (strcmp(save_dollar, "$") == 0) {		// $<VAR>=<value>
				int new = checkDollar(arr, var_dollar, cPos, vPos, invalidCheck);
				if (new == 0) {	// new var, increment pos.
					vPos++;
				}
			cPos++;
			} else {
				// Check if non-builtin cmd exist
				pipe(fds);
				pid_child = fork();
				// int flag = 0;
				char *cmds = strdup(arr[0]);
				char *logCmd = strdup(arr[0]);
				// char *a;
				// printf("%s\n", arr[pos-1]);
				// arr[pos] = '\0'; // null terminated (end of array)

				//child process
				int status;
				if (pid_child == 0) { 
					dup2(fds[1], STDOUT_FILENO);		// write to pipe
					dup2(fds[1], STDERR_FILENO); 		// write to pipe
					close(fds[0]);
					close(fds[1]);			
					if (execvp(cmds, arr) == -1) {
						char *m = "Missing keyword or command, or permission problem";
						int m_len = strlen(m);
						write(STDERR_FILENO, m, m_len);	// write to shell
						printf("\n");
						exit(1);
					}
				} 
				// parent process
				else { 
					char str[1];
					close(fds[1]);
					wait(&status);
					if (WEXITSTATUS(status) == 1) {
						if (invalidCheck == 1) { //script mode invalid cmd
							printf("Missing keyword or command, or permission problem in Script Mode. Bye!\n");
							exit(0);
						}
						history(logCmd, 1, cPos);
						cPos++;
					} else {
						history(logCmd, 0, cPos);
						cPos++;
					}

					while (read(fds[0], str, 1) > 0) { 	// read from pipe
						write(1, str, 1); 				// write to shell
					}
					close(fds[0]);	
				}
			}
		}
	}
}
