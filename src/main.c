#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define VERSION		"0.1.0"

bool getNextString(FILE *file, char *newString);
bool compareLines(FILE *file1, FILE *file2, size_t *lines, char *str1, char *str2);
void printDifferences(char *diffStr1, char *diffStr2, size_t fromLine, size_t toLine);

void usage(void);
void die(const char *fmt, ...);

bool gIsLastLine;
size_t unmatchingLines;

int main(int argv, char *argc[]) {
	size_t i, j;
	char *filename1, *filename2;

	filename1 = NULL;
	filename2 = NULL;

	/* parses through the command line arguments */
	for(i = 1; i < argv; i++) {
		/* all options start with - */
		if(*argc[i] == '-') {
			/* -v prints out version and quits*/
			if(!strcmp(argc[i], "-v")) {
				printf("%s\n", VERSION);
				return EXIT_SUCCESS;
			}
		}

		/* if an argument doesn't start with - then it's one of the two
		 * filenames */
		else {
			if(filename1 == NULL)
				filename1 = argc[i];
			else if(filename2 == NULL)
				filename2 = argc[i];
			else
				die("can't compare more than two files");
		}
	}

	/* if the filenames weren't provided (meaning that the filename
	 * variables are still NULL) then prints out help and exits */
	if(filename1 == NULL || filename2 == NULL) {
		usage();
	}

	FILE *file, *file2;
	char *str, *str2;

	file = fopen(filename1, "r");
	file2 = fopen(filename2, "r");
	str = (char *)malloc(4096);
	str2 = (char *)malloc(4096);

	*str = '\0';
	*str2 = '\0';

	/* checks if any of the pointers are null */
	if(str == NULL || str == NULL || file == NULL || file2 == NULL)
		return EXIT_FAILURE;

	i = 0;

	/* program main loop */
	for(;;) {
		/* exits main loop if the previous line had the EOF character */
		if(gIsLastLine)
			break;
		j = i;
		/* compares lines and prints out differences if there are any*/
		if(compareLines(file, file2, &i, str, str2)) {
			printDifferences(str, str2, j + 1, i);
		}

		/* places the "\0" / EOF character in the start of the strings so that they
		 * can be used again (basically deletes the previous contents of the string) */
		*str = '\0';
		*str2 = '\0';
	}

	fclose(file);
	fclose(file2);

	return EXIT_SUCCESS;
}

/* this function reads the file "file" and fills the contents of newString
 * with the contents of the file "file" until a newline character is found
 * or a EOF character is encountered. If a EOF character is encountered
 * the program returns the value "true" / 1 otherwise returns "false" / 0 */
bool getNextString(FILE *file, char *newString) {
	char ch;
	size_t i;

	i = 0;

//	printf("%p: \n", file);

	for(;;) {
		ch = fgetc(file);

		if(ch == EOF) {
			puts("EOF found");
			return true;
		}

//		putchar(ch);

		*(newString+i++) = ch;

		if(ch == '\n')
			break;
	}

	*(newString+i) = '\0';

	return false;
}

bool compareLines(FILE *file1, FILE *file2, size_t *lines, char *str1, char *str2) {
	size_t i;
	bool comparation;
	char *newStr1,
	     *newStr2;

	newStr1 = (char *)malloc(4096);
	newStr2 = (char *)malloc(4096);

	gIsLastLine = false;

	i = 0;

	for(;;) {
		if(getNextString(file1, newStr1) || getNextString(file2, newStr2)) {
			gIsLastLine = true;
			puts("NO");
			printf("%d\n", gIsLastLine);
		}

		(*lines)++;
		i++;

		if(strcmp(newStr1, newStr2)) {
			comparation = true;

			strcat(str1, newStr1);
			strcat(str2, newStr2);
		}

		else
			break;

		if(gIsLastLine || i > 10)
			break;
	}

	free(newStr1);
	free(newStr2);

	return comparation;
}

void printDifferences(char *diffStr1, char *diffStr2, size_t fromLine, size_t toLine) {
	size_t i;

	if(toLine == fromLine)
		printf("differences found from line %ld\n", fromLine); 
	else
		printf("differences found from line %ld to %ld\n", fromLine, toLine);

	printf("\n< ");

	i = 0;

	while(*(diffStr1+i) != EOF && *(diffStr1+i) != 0) {
		putchar(*(diffStr1+i));

		if(*(diffStr1+i++) == '\n' && *(diffStr1+i) != 0)
			printf("< ");
	}

	printf("\n> ");

	i = 0;

	while(*(diffStr2+i) != EOF && *(diffStr2+i) != 0) {
		putchar(*(diffStr2+i));

		if(*(diffStr2+i++) == '\n' && *(diffStr2+i) != 0)
			printf("> ");
	}

	printf("\n\n");
}

void usage(void) {
	fputs("usage: midiff [-v] file1 file2\n", stderr);
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
