#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define VERSION		"0.1.0"

struct contextList {
	char *str;
	struct contextList *next;
};

bool getNextString(FILE *file, char *newString);
bool compareLines(FILE *file1, FILE *file2, size_t *lines, char *str1, char *str2, struct contextList **listHead);
void printDifferences(char *diffStr1, char *diffStr2,
		size_t fromLine, size_t toLine, struct contextList *contextListHead);

struct contextList *addToContextList(char *str, struct contextList *listHead);
size_t lenContextList(struct contextList *listEntry);
void printContextList(struct contextList *listHead);
void deleteContextList(struct contextList *listHead);

void usage(void);
void die(const char *fmt, ...);

bool gIsLastLine;
size_t gUnmatchingLines,
       gContextLines;


int main(int argv, char *argc[]) {
	size_t i, j;
	char *filename1, *filename2;

	filename1 = NULL;
	filename2 = NULL;

	gContextLines = 0;

	/* parses through the command line arguments */
	for(i = 1; i < argv; i++) {
		/* all options start with - */
		if(*argc[i] == '-') {
			if(!strcmp(argc[i], "-h"))
				usage();
			if(!strcmp(argc[i], "-c"))
				gContextLines = atoi(argc[++i]);
		}

		/* if an argument doesn't start with "-" then it's one of the two
		 * filenames */
		else {
			if(filename1 == NULL) {
				filename1 = argc[i];
				if(filename1 == NULL)
					die("memory allocation failed");
			}

			else if(filename2 == NULL) {
				filename2 = argc[i];
				if(filename2 == NULL)
					die("memory allocation failed");
			}

			else
				die("can't compare more than two files");
		}
	}

	printf("\t%d\n", gContextLines);

	/* if the filenames weren't provided (meaning that the filename
	 * variables are still NULL) then prints out help and exits */
	if(filename1 == NULL || filename2 == NULL)
		usage();

	FILE *file1, *file2;
	char *str1, *str2;
	struct contextList *contextListHead;

	file1 = fopen(filename1, "r");
	file2 = fopen(filename2, "r");
	str1 = (char *)malloc(4096);
	str2 = (char *)malloc(4096);

	contextListHead = NULL;

	*str1 = '\0';
	*str2 = '\0';

	gUnmatchingLines = 0;

	/* checks if any of the pointers are null */
	if(str1 == NULL || str2 == NULL || file1 == NULL || file2 == NULL)
		die("memory allocation failed");

	i = 0;

	/* program main loop */
	for(;;) {
		/* exits main loop if the previous line had the EOF character */
		if(gIsLastLine)
			break;
		j = i;
		/* compares lines and prints out differences if there are any*/
		if(compareLines(file1, file2, &i, str1, str2, &contextListHead)) {
			printDifferences(str1, str2, j + 1, i, contextListHead);
			deleteContextList(contextListHead);

			contextListHead = NULL;
		}

		/* places the "\0" / EOF character in the start of the strings so that they
		 * can be used again (basically deletes the previous contents of the string) */
		*str1 = '\0';
		*str2 = '\0';
	}

	printf("File has been succesfully compared %d / %d lines we're different\n", 
			gUnmatchingLines, i);

	fclose(file1);
	fclose(file2);

	deleteContextList(contextListHead);

	contextListHead = NULL;

	return EXIT_SUCCESS;
}

/* This function reads the file "file" and fills the contents of newString
 * with the contents of the file "file" until a newline character is found
 * or a EOF character is encountered. If a EOF character is encountered
 * the program returns the value "true" / 1 otherwise returns "false" / 0 */
bool getNextString(FILE *file, char *newString) {
	char ch;
	size_t i;

	i = 0;

	for(;;) {
		ch = fgetc(file);

		if(ch == EOF)
			return true;

		*(newString+i++) = ch;

		if(ch == '\n')
			break;
	}

	/* Last character of a string must be EOF or '\0' */
	*(newString+i) = '\0';

	return false;
}

/* Compare two lines and if the lines compared are different then go to the
 * next line and check if that's different and so on. */
bool compareLines(FILE *file1, FILE *file2, size_t *lines, 
		char *str1, char *str2,
		struct contextList **listHead) {
	size_t i;
	bool comparison;
	char *newStr1,
	     *newStr2;

	newStr1 = (char *)malloc(4096);
	newStr2 = (char *)malloc(4096);

	comparison = false;

	gIsLastLine = false;

	i = 0;

	size_t a;

	for(;;) {
		/* Checks if the line contained an EOF (end of file) */
		gIsLastLine = getNextString(file1, newStr1) || getNextString(file2, newStr2);

		(*lines)++;
		i++;

		/* Compares lines */
		if(strcmp(newStr1, newStr2)) {
			comparison = true;

			gUnmatchingLines++;

			strcat(str1, newStr1);
			strcat(str2, newStr2);
		}
		else {
			if(gContextLines != 0 && !comparison) {
				*listHead = addToContextList(newStr1, *listHead);
			}

			break;
		}

		if(gIsLastLine || i > 40)
			break;
	}

	return comparison;
}

void printDifferences(char *diffStr1, char *diffStr2, size_t fromLine, size_t toLine,
		struct contextList *contextListHead) {
	size_t i;

	if(toLine == fromLine)
		printf("differences found from line %ld\n", fromLine); 
	else
		printf("differences found from line %ld to %ld\n", (fromLine), toLine);

	if(gContextLines != 0 && contextListHead != NULL) {
		struct contextList *listEntry;

		listEntry = contextListHead;

		putchar('\n');

		for(i = 0; i < lenContextList(contextListHead); i++) {
			printf("C %s", listEntry->str);

			listEntry = listEntry->next;
		}
	}

	i = 0;

	printf("\n< ");

	while(*(diffStr1+i) != EOF && *(diffStr1+i) != 0) {
		putchar(*(diffStr1+i));

		if(*(diffStr1+i++) == '\n' && *(diffStr1+i) != 0)
			printf("< ");
	}

	i = 0;

	printf("\n");

	printf("> ");

	while(*(diffStr2+i) != EOF && *(diffStr2+i) != 0) {
		putchar(*(diffStr2+i));

		if(*(diffStr2+i++) == '\n' && *(diffStr2+i) != 0)
			printf("> ");
	}

	printf("\n\n");
}

struct contextList *addToContextList(char *str, struct contextList *listHead) {
	struct contextList *listEntry;

	if(lenContextList(listHead) >= gContextLines) {
		printf("lenContextList %d >= %d\n", lenContextList(listHead), gContextLines);
		
		listEntry = listHead->next;

		free(listHead);
		free(listHead->str);

		listHead = listEntry;
	}

	if(listHead == NULL) {
		listHead = (struct contextList *)malloc(sizeof(*listHead));

		listEntry = listHead;
	}

	else {
		listEntry = listHead;

		while(listEntry->next != NULL) {
			listEntry = listEntry->next;
		}

		listEntry->next = (struct contextList *)malloc(sizeof(*listHead)); 
		listEntry = listEntry->next;
	}

	listEntry->str = str;
	listEntry->next = NULL;

	return listHead;
}

size_t lenContextList(struct contextList *listEntry) {
	size_t i;

	i = 0;

	while(listEntry != NULL) {
		listEntry = listEntry->next;
		i++;
	}

	return i;
}

void printContextList(struct contextList *listHead) {
	struct contextList *listEntry;
	size_t i;

	listEntry = listHead;

	i = 0;

	while(listEntry != NULL) {
		printf("listEntry %02X: address: %p str: %s\n", i, listEntry, listEntry->str);
		listEntry = listEntry->next;
		i++;
	}
}

void deleteContextList(struct contextList *listHead) {
	struct contextList *listEntry, *temp;
	size_t i;

	if(listHead == NULL)
		return;

	while(listHead->next != NULL) {
		listEntry = listHead;

		while(listEntry->next != NULL) {
			temp = listEntry;
			listEntry = listEntry->next;
		}

		temp->next = NULL;

		free(listEntry->str);
		free(listEntry);
	}

	free(listHead);
}

void usage(void) {
	fputs("usage: midiff [-c context_lines] file1 file2\n", stderr);

	exit(EXIT_SUCCESS);
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
