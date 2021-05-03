#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

int main() {

	DIR *dir = opendir("/proc");
	struct dirent *read;

	char *data;
	char *prev = "/proc/";
	char *after = "/cmdline";
	int length;

	FILE *file;

	// readdir(): returns pointer to next directory entry in dir
    while ((read = readdir(dir)) != NULL) {
    	bool flag = false;
    	data = read->d_name; //d_name: directory filename
		length = strlen(data);

    	for(int i = 0; i < length; i++) {
    		if(isdigit(data[i])) {
    			flag = true;
    		} else {
    			flag = false;
    			break;
    		}
    	}
    	//flag signifies pid for path usage
    	if (flag == true) {
    		int pid = atoi(data);
	    	char *result = malloc(strlen(data) + strlen(prev) + strlen(after) + 1); //malloc spacing +1 for null-terminator
	    	
	    	strcpy(result, prev);
	    	strcat(result, data);
	    	strcat(result, after); //contains path

		    file = fopen(result, "r");
		    free(result);
		    char string[200];
		    fgets(string, 200, file);

		    // string does not contain "/" path
		    if (string[0] != '/')
		    {
		    	if (string[0] == '.') {
		    		char *last = strrchr(string, '/');
		    		// string starts with "." with immediate "/" if true
					if (last && *(last + 1)) {
					    printf("%d %s", pid, last + 1);
					}
		    	} else {
			    	printf("%d %s", pid, string);	    		
		    	}

		    } else { 
			    char *last = strrchr(string, '/'); // start at last occuring "/"
				if (last && *(last + 1)) {
				    printf("%d %s", pid, last + 1);
				} else {
				    printf("%d %s", pid, string);
				}
		    }
			fclose(file);
			printf("\n");
    	}
    }
        
	closedir(dir);
	return(0);
}

// resource:
// strrchr usage to find last occuring word: https://stackoverflow.com/questions/9219940/traversing-c-string-get-the-last-word-of-a-string