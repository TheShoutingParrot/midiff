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

	i = 0;

	for(;;) {
		if(getNextString(file, str))
			return EXIT_SUCCESS;
		if(getNextString(file2, str2))
			return EXIT_SUCCESS;

		printf("%d\n", strcmp(str, str2));
		if(!strcmp(str, str2))
			printf("%06ld>\n%s\n%s\n", i, str, str2);

		i++;
	}

	fclose(file);

	return EXIT_SUCCESS;
}

bool getNextString(FILE *file, char *newString) {
	char ch;
	size_t i;

	i = 0;

	for(;;) {
		ch = fgetc(file);

		if(ch == EOF) {
			printf("eof found %s\n", newString);
			return true;
		}
		if(ch == '\n')
			break;

		*(newString+i++) = ch;
	}

	*(newString+i) = '\0';

	return false;
}

void usage(void) {
	fputs("usage: midiff [-v] file1 file2", stderr);
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

