#include "midiff.h"

int main(int argv, char *argc[]) {
	size_t i;
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

	/* if the filenames weren't provided (meaning that the filename
	 * variables are still NULL) then prints out help and exits */
	if(filename1 == NULL || filename2 == NULL)
		usage();

	FILE *file1, *file2;
	char *limbo;
	struct list *contextListHead,
		    *diff1ListHead,
		    *diff2ListHead;

	file1 = fopen(filename1, "r");
	file2 = fopen(filename2, "r");

	contextListHead = NULL;
	diff1ListHead = NULL;
	diff2ListHead = NULL;

	limbo = NULL;

	gUnmatchingLines = 0;

	/* checks if any of the pointers are null (that aren't supposed to be) */
	if(file1 == NULL || file2 == NULL)
		die("memory allocation failed");

	i = 0;

	/* program main loop */
	for(;;) {
		/* exits main loop if the previous line had the EOF character */
		if(gIsLastLine)
			break;
		size_t j = i;

		/* compares lines and prints out differences if there are any and finalle
		 * frees/deletes the linked lists */
		if(compareLines(file1, file2, &i, &diff1ListHead, &diff2ListHead, &contextListHead, &limbo)) {
			printDifferences(diff1ListHead, diff2ListHead, j + 1, i - 1, contextListHead);

			deleteList(contextListHead);
			deleteList(diff1ListHead);
			deleteList(diff2ListHead);

			contextListHead = NULL;
			diff1ListHead = NULL;
			diff2ListHead = NULL;
		}
	}

	printf("File has been succesfully compared %lu / %lu lines we're different\n", 
			gUnmatchingLines, i);

	fclose(file1);
	fclose(file2);

	deleteList(contextListHead);
	deleteList(diff1ListHead);
	deleteList(diff2ListHead);

	return EXIT_SUCCESS;
}
