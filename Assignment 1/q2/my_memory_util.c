#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>

int main() {

	FILE *file;

	file = fopen("/proc/meminfo", "r");
    char string[200];
    char word[200];
    char tot[200];
    char free[200];
    char buffer[200];
    char cache[200];
    char slab[200];
    
    while(fgets(string, 200, file) != NULL) {
    	sscanf(string,"%s",word); // get first word in each line

    	if(strcmp(word, "MemTotal:") == 0) {
    		int j = 0;
    		for (int i = 0; i < strlen(string); ++i) {
                //extract number from line
    			if (string[i] >= '0' && string[i] <= '9') {
    				tot[j] = string[i];
    				j++;
    			}
    		}
    		// printf("%s\n", tot);
    	} else if (strcmp(word, "MemFree:") == 0){
    		int j = 0;
    		for (int i = 0; i < strlen(string); ++i) {
    			if (string[i] >= '0' && string[i] <= '9') {
    				free[j] = string[i];
    				j++;
    			}
    		}
    		// printf("%s\n", free);
    	} else if (strcmp(word, "Buffers:") == 0){
    		int j = 0;
    		for (int i = 0; i < strlen(string); ++i) {
    			if (string[i] >= '0' && string[i] <= '9') {
    				buffer[j] = string[i];
    				j++;
    			}
    		}
    		// printf("%s\n", buffer);
    	} else if (strcmp(word, "Cached:") == 0){
    		int j = 0;
    		for (int i = 0; i < strlen(string); ++i) {
    			if (string[i] >= '0' && string[i] <= '9') {
    				cache[j] = string[i];
    				j++;
    			}
    		}
    		// printf("%s\n", cache);
    	} else if (strcmp(word, "Slab:") == 0){
    		int j = 0;
    		for (int i = 0; i < strlen(string); ++i) {
    			if (string[i] >= '0' && string[i] <= '9') {
    				slab[j] = string[i];
    				j++;
    			}
    		}
    		// printf("%s\n", slab);
    	}
    }
	fclose(file);

    float totalMem = atoi(tot);
    float freeMem = atoi(free);
    float buffers = atoi(buffer);
    float cached = atoi(cache);
    float slabb = atoi(slab);

    float mem_utilization = (totalMem - freeMem - buffers - cached - slabb);
    mem_utilization = mem_utilization/totalMem;
    mem_utilization = mem_utilization * 100;
    printf("MEMORY UTILIZATION: %f\n", mem_utilization);

	return 0;
}
// reference: https://stackoverflow.com/questions/3756308/how-to-read-only-the-first-word-from-each-line
// scanf ignores first space using %s and gets the first word.
// s read subsequent characters until a whitespace is found: https://www.tutorialspoint.com/c_standard_library/c_function_sscanf.htm