#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

int main()
{
	FILE *name;
	name = fopen("/etc/os-release", "r");

	char string[200];
	char word[200];
	int j = 0;

	FILE *kernel;
	char string2[200];
	char word2[200];

    while(fgets(string, 200, name) != NULL) {
    	// sscanf(string,"%s",word);
    	// printf("%s\n", word);

    	//strstr finds first occurence of substring
    	if (strstr(string, "PRETTY_NAME=")) {
    		// printf("%s\n", string);
    		char *p = strchr(string, '"'); // get word surrounded by quotations
    		int len = strlen(p);		
    		for (int i = 0; i < len; ++i)
    		{
    			if (p[i] != '"') { // removes quotations around word
    				word[j] = p[i];
    				j++;
    			}
    		}
    	}
    }

    fclose(name);
   	printf("OS: %s", word);

	kernel = fopen("/proc/version", "r");
	fgets(string2, 400, kernel); // first line
	int space_counter = 0;
	int k = 0;
	int l = 0;

	//include everything until third space is reached
	while (space_counter != 3) {
		if (string2[l] == ' ')
		{
			space_counter++;
			if (space_counter != 3) 
			{
				word2[k] = string2[l];
				k++;
				l++;
			}
		} else {
			word2[k] = string2[l];
			k++;
			l++;
		}
	}


	fclose(kernel);
	printf("%s\n", word2);
	return 0;
}

// Reference
// https://stackoverflow.com/questions/15515088/how-to-check-if-string-starts-with-certain-string-in-c/15515276