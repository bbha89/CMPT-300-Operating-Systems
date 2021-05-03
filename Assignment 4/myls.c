#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <dirent.h> // open directories

#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

#include <time.h>

#include <pwd.h>
#include <grp.h>

void printCurDirectory(char *path) {
	
	struct stat checkFile;
	stat(path, &checkFile);
	// check if file path is a regular file
	if (S_ISREG(checkFile.st_mode) || S_ISLNK(checkFile.st_mode)) {
		printf("%s\n", path);
	} else {

		struct dirent** dir;
		int n = scandir(path, &dir, NULL, alphasort);

		int i = 0;

		while (i < n) {
			char *data = dir[i]->d_name;
	    	if (data[0] != '.') {
	    		printf("%s\n", data);
	    	} 

    		free(dir[i]);
    		i++;
		}
	}

}

void printINode(char *path, int checkR, int onlyR) {
	
	struct stat checkFile;

	// check if filepath exist
	stat(path, &checkFile);
	// check if file path is a regular file
	if (S_ISREG(checkFile.st_mode) || S_ISLNK(checkFile.st_mode)) {
		printf("%6d %s\n", (int) checkFile.st_ino, path);
	} else {

		struct dirent** dir;
		int n;
		n = scandir(path, &dir, NULL, alphasort);

		int i = 0;

		while (i < n) {
			char *data = dir[i]->d_name;
			char *path2 = malloc(sizeof(dir[i]->d_name));
	    	if (data[0] != '.') {

				struct stat fileStat;

	       		strcpy(path2, path);
	    		strcat(path2, "/");
	    		strcat(path2, dir[i]->d_name);
		    	stat(path2, &fileStat);

					if (onlyR == 0) {
	    			printf("%6d %s\n", (int)dir[i]->d_ino, dir[i]->d_name);
	    		} else {
	    			printf("%s\n", dir[i]->d_name);
	    		}

	    		if ((onlyR == 1 || checkR == 1) && S_ISDIR(fileStat.st_mode)) {
	    			printINode(path2, checkR, onlyR);
	    		} 


	    	}
	    	free(path2);
    		free(dir[i]);
    		i++;
		}

	}
} 

void getPermissions(char* permissions, struct stat fileStat) {

	// Check if file is a directory or not
	if (S_ISDIR(fileStat.st_mode)) {
		strcat(permissions, "d");
	} else {
		strcat(permissions, "-");
	}

	// Owner permissions
	if (fileStat.st_mode & S_IRUSR) {
		strcat(permissions, "r");
	} else {
		strcat(permissions, "-");
	}
	if (fileStat.st_mode & S_IWUSR) {
		strcat(permissions, "w");
	} else {
		strcat(permissions, "-");
	}
	if (fileStat.st_mode & S_IXUSR) {
		strcat(permissions, "x");
	} else {
		strcat(permissions, "-");
	}
	// Group permissions
	if (fileStat.st_mode & S_IRGRP) {
		strcat(permissions, "r");
	} else {
		strcat(permissions, "-");
	}
	if (fileStat.st_mode & S_IWGRP) {
		strcat(permissions, "w");
	} else {
		strcat(permissions, "-");
	}
	if (fileStat.st_mode & S_IXGRP) {
		strcat(permissions, "x");
	} else {
		strcat(permissions, "-");
	}
	// Others permissions
	if (fileStat.st_mode & S_IROTH) {
		strcat(permissions, "r");
	} else {
		strcat(permissions, "-");
	}
	if (fileStat.st_mode & S_IWOTH) {
		strcat(permissions, "w");
	} else {
		strcat(permissions, "-");
	}
	if (fileStat.st_mode & S_IXOTH) {
		strcat(permissions, "x");
	} else {
		strcat(permissions, "-");
	}
}


void printlNode(char *path, int checkIL, int checkR) {
	struct stat checkFile;

	stat(path, &checkFile);
	// check if file path is a regular file
	if (S_ISREG(checkFile.st_mode) || S_ISLNK(checkFile.st_mode)) {

		char perm[200];
		memset(perm, '\0', 200);
    	getPermissions(perm, checkFile);

    	// Number of hard links
    	int links = checkFile.st_nlink;

    	// User ID/Group ID of file
		struct passwd *userName = getpwuid(checkFile.st_uid);
		struct group *groupName = getgrgid(checkFile.st_gid);

		// file size
		int fileSize = checkFile.st_size;

		// Last modified
		struct tm *time = localtime(&checkFile.st_mtime);
		char lastModified[200];
		memset(lastModified, '\0', 200);
		strftime(lastModified, 200, "%b %2d %4Y %02H:%02M", time);

		if (checkIL == 1) {
			printf("%d %s %d %s %s %d %s %s\n", (int)checkFile.st_ino, perm, links, userName->pw_name, groupName->gr_name, fileSize, lastModified, path);

		} else {
			printf("%s %d %s %s %d %s %s\n", perm, links, userName->pw_name, groupName->gr_name, fileSize, lastModified, path);
		}
	} else {

		struct dirent** dir;
		int n = scandir(path, &dir, NULL, alphasort);

		int i = 0;

		while (i < n) {
			char *data = dir[i]->d_name;
	    	if (data[0] != '.') {

				struct stat fileStat;

				char *path2 = malloc(sizeof(dir[i]->d_name));
	       		strcpy(path2, path);
	    		strcat(path2, "/");
	    		strcat(path2, data);
		    	stat(path2, &fileStat);

				// Permissions
				char perm[200];
				memset(perm, '\0', 200);
		    	getPermissions(perm, fileStat);

		    	// Number of hard links
		    	int links = fileStat.st_nlink;

		    	// User ID/Group ID of file
				struct passwd *userName = getpwuid(fileStat.st_uid);
				struct group *groupName = getgrgid(fileStat.st_gid);

				// file size
				int fileSize = fileStat.st_size;

				// Last modified
				struct tm *time = localtime(&fileStat.st_mtime);
				char *lastModified = malloc(100);

				strftime(lastModified, 100, "%b %2d %4Y %02H:%02M", time);

				if (checkIL == 1) {
				    	printf("%d %s %d %s %s %d %s %s\n", (int)dir[i]->d_ino, perm, links, userName->pw_name,
				    							groupName->gr_name, fileSize, lastModified, data);
				    	if (checkR == 1 && S_ISDIR(fileStat.st_mode)) {
				    		printlNode(path2, checkIL, checkR);
				    	}
				    }
				else {
				    printf("%s %d %s %s %d %s %s\n", perm, links, userName->pw_name,
				    							groupName->gr_name, fileSize, lastModified, data);
						if (checkR == 1 && S_ISDIR(fileStat.st_mode)) {
				    		printlNode(path2, checkIL, checkR);
				    	}
				}
		    	free(path2);
		    	free(lastModified);
	    	}
    		free(dir[i]);
    		i++;
		}
	}
}

void checkError(char *buff2) {
	struct stat checkExist;
	DIR * dir = opendir(buff2);
	if (dir != NULL) {
		closedir(dir);
	} else if (stat(buff2, &checkExist) == 0) {
		closedir(dir);
	} else {
		printf("File/Directory does not exist\n");
		exit(1);
	}

}


int main(int argc, char const *argv[])
{

	// Print current directory, no arguments provided
	if (argc < 2) {
		printCurDirectory(".");
	} 
	// 1 or more arguments provided
	else {
		// Checks if input contains one of the arguments {i,l,R}
		int i = 0;
		int l = 0;
		int R = 0;
		int fileIndex = 1;

		// checks the beginning of each argument for '-' and end on file argument
		int index = 1;
		while (fileIndex < argc && argv[fileIndex][0] == '-') {
			// checks each char after '-' for i,l,R

			char buff[200];
			strcpy(buff, argv[index]);

			int argSize = strlen(buff);

			if (argSize == 1) {
				printf("myls: invalid option after -\n");
				exit(1);
			}

			for (int j = 1; j < argSize; ++j) {
				char c = buff[j];
				if (c == 'i') {
					i = 1;
				} else if (c == 'l') {
					l = 1;
				} else if (c == 'R') {
					R = 1;
				} 
				// Option is not valid
				else {
					printf("myls: invalid option after -\n");
					exit(1);
				}
			}
			fileIndex++;
			index++;
		} 

		int fileArguments = 0;

		if (fileIndex < argc) {
			// file arguments were entered
			fileArguments = 1;
		}

		// Options & No file arguments
		if (i == 1 || l == 1 || R == 1) {
			int checkIL = 0;
			int checkR = 0;
			int onlyR = 0;
			if (i == 1 && l == 1 && R == 1) {
				checkR = 1;
				checkIL = 1;
				if (fileArguments == 1) {
					int space = 0;
					while (fileIndex < argc) {
						char buff2[200];
						if (space > 0) {
							printf("\n");
						}
						strcpy(buff2, argv[fileIndex]);
						checkError(buff2);
						printf("%s: \n", buff2);
						printlNode(buff2, checkIL, checkR);
						fileIndex++;
						space++;
					}
				} else {
					printlNode(".", checkIL, checkR);
				}
			}
			else if (i == 1 && l == 1) {
				checkIL = 1;
				if (fileArguments == 1) {
					int space = 0; 
					while (fileIndex < argc) {
						if (space > 0) {
							printf("\n");
						}
						char buff2[200];
						strcpy(buff2, argv[fileIndex]);
						checkError(buff2);
						printf("%s: \n", buff2);
						printlNode(buff2, checkIL, checkR);
						fileIndex++;
						space++;
					}
				} else {
					printlNode(".", checkIL, checkR);
				}

			} else if (l == 1 && R == 1) {
				checkR = 1;
				if (fileArguments == 1) {
					int space = 0;
					while (fileIndex < argc) {
						char buff2[200];
						if (space > 0) {
							printf("\n");
						}
						strcpy(buff2, argv[fileIndex]);
						checkError(buff2);
						printf("%s: \n", buff2);
						printlNode(buff2, checkIL, checkR);
						fileIndex++;
						space++;
					}
				} else {
					printlNode(".", checkIL, checkR);
				}
			} else if (i == 1 && R == 1) {
				checkR = 1;
				if (fileArguments == 1) {
					int space = 0;
					while (fileIndex < argc) {
						char buff2[200];
						if (space > 0) {
							printf("\n");
						}
						strcpy(buff2, argv[fileIndex]);
						checkError(buff2);
						printf("%s: \n", buff2);
						printINode(buff2, checkR, onlyR);
						fileIndex++;
						space++;
					}
				} else {
					printINode(".", checkR, onlyR);
				}
			}


			else {
				if (i == 1) {

					if (fileArguments == 1) {
						int space = 0;
						while (fileIndex < argc) {
							if(space > 0) {
								printf("\n");
							}
							char buff2[200];
							strcpy(buff2, argv[fileIndex]);

							checkError(buff2);
							printf("%s: \n", buff2);
							printINode(buff2, checkR, onlyR);
							fileIndex++;
							space++;
							}
						} else {
						printINode(".", checkR, onlyR);
					}
				} else if (l == 1) {
					if (fileArguments == 1) {
						int space = 0;
						while (fileIndex < argc) {
							if(space > 0) {
								printf("\n");
							}
							char buff2[200];
							strcpy(buff2, argv[fileIndex]);
							checkError(buff2);
							printf("%s: \n", buff2);
							printlNode(buff2, checkIL, checkR);
							fileIndex++;
							space++;
						}
					} else {
						printlNode(".", checkIL, checkR);
					}
				} 
				else if (R == 1) {
					checkR = 1;
					onlyR = 1;
					if (fileArguments == 1) {
						int space = 0;
						while (fileIndex < argc) {
							if(space > 0) {
								printf("\n");
							}
							char buff2[200];
							strcpy(buff2, argv[fileIndex]);
							checkError(buff2);
							printf("%s: \n", buff2);
							printINode(buff2, checkR, onlyR);
							fileIndex++;
							space++;
						}
					} else {
						printINode(".", checkR, onlyR);
					}
				}
			}
		} 

		else {
		// File arguments & no Options
		// print the good files/folders until the bad one, print an error message and then exit
			int space = 0;
			while (fileIndex < argc) {
				if(space > 0) {
					printf("\n");
				}
				char buff2[200];
				strcpy(buff2, argv[fileIndex]);
				checkError(buff2);
				printCurDirectory(buff2);

				fileIndex++;
				space++;
			}
		}

	}
	return 0;
}

