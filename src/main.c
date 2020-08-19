#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

void getNextString(FILE *file, char *newString);
bool doStringsMatch(char *str1, char *str2);

int main(int argv, char *argc[]) {
	FILE *file, *file2;
	char *str, *str2;

	file = fopen("log.txt", "r");
	file2 = fopen("log2.txt", "r");
	str = (char *)malloc(4096);
	str2 = (char *)malloc(4096);

	if(str == NULL || str == NULL || file == NULL || file2 == NULL)
		return EXIT_FAILURE;

	getNextString(file, str);
	getNextString(file2, str2);

	puts(str);

	fclose(file);

	return EXIT_SUCCESS;
}

void getNextString(FILE *file, char *newString) {
	char ch;
	size_t i;

	i = 0;

	while((ch = fgetc(file)) != EOF) {
		if(ch == '\n')
			break;
		*(newString+i++) = ch;
	}

	*(newString+i) = '\0';

	if(!realloc(newString, strlen(newString)))
		exit(EXIT_FAILURE);
}

bool doStringsMatch(char *str1, char *str2) {
	return !strcmp(str1, str2);
}
