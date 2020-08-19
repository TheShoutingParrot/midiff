#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define VERSION		"0.1.0"

bool getNextString(FILE *file, char *newString);
bool doStringsMatch(char *str1, char *str2);

void usage(void);
void die(const char *fmt, ...);

int main(int argv, char *argc[]) {
	size_t i;
	char *filename1, *filename2;

	filename1 = NULL;
	filename2 = NULL;

	for(i = 1; i < argv; i++) {
		if(*argc[i] == '-') {
			if(!strcmp(argc[i], "-v")) {
				printf("%s\n", VERSION);
				return EXIT_SUCCESS;
			}
		}

		else {
			if(filename1 == NULL)
				filename1 = argc[i];
			else if(filename2 == NULL)
				filename2 = argc[i];
			else
				die("can't compare more than two files");
		}
	}

	if(filename1 == NULL || filename2 == NULL) {
		usage();
	}

	FILE *file, *file2;
	char *str, *str2;

	file = fopen(filename1, "r");
	file2 = fopen(filename2, "r");
	str = (char *)malloc(4096);
	str2 = (char *)malloc(4096);

	if(str == NULL || str == NULL || file == NULL || file2 == NULL)
		return EXIT_FAILURE;

	getNextString(file, str);
	getNextString(file2, str2);

	puts(str);
	puts(str2);

	fclose(file);

	return EXIT_SUCCESS;
}

bool getNextString(FILE *file, char *newString) {
	char ch;
	size_t i;

	i = 0;

	for(;;) {
		ch = fgetc(file);

		if(ch == EOF)
			return true;
		if(ch == '\n')
			break;

		*(newString+i++) = ch;
	}

	*(newString+i) = '\0';

	if(!realloc(newString, strlen(newString)))
		exit(EXIT_FAILURE);

	return false;
}

bool doStringsMatch(char *str1, char *str2) {
	return !strcmp(str1, str2);
}

void die(const char *fmt, ...) {
        va_list vargs;

        va_start(vargs, fmt);

	fprintf(stderr, "FATAL ERROR: ");
        vfprintf(stderr, fmt, vargs);

        fputc('\n', stderr);

        va_end(vargs);

        exit(EXIT_FAILURE);
}

