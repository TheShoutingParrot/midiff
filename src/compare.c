#include "midiff.h"

/* This function reads the file "file" and fills the contents of newString
 * with the contents of the file "file" until a newline character is found
 * or a EOF character is encountered. If a EOF character is encountered
 * the program returns the value "true" / 1 otherwise returns "false" / 0 */
bool getNextString(FILE *file, char *newString) {
        size_t i;

        i = 0;

        for(;;) {
                char ch = fgetc(file);

                if(ch == EOF) {
			if(ferror(file))
				die("I/O error encountered when reading");

                        return true;
		}

                if(ch == '\n')
                        break;

                *(newString+i++) = ch;
        }

        /* Last character of a string must be EOF */
        *(newString+i) = '\0';

        return false;
}

/* Compare two lines and if the lines compared are different then go to the
 * next line and check if that's different and so on. */
bool compareLines(FILE *file1, FILE *file2, size_t *lines,
                struct list **diff1, struct list **diff2,
                struct list **context,
                char **limbo) {
        size_t i, j;
        bool comparison;

        comparison = false;

        gIsLastLine = false;

        i = 0;
	j = 0;

        for(;;) {
                char *newStr1 = (char *)calloc(256, sizeof(char));
                char *newStr2 = (char *)calloc(256, sizeof(char));

                /* Checks if the line contained an EOF (end of file) */
                gIsLastLine = getNextString(file1, newStr1) || getNextString(file2, newStr2);

                (*lines)++;
                i++;

		if(gIsLastLine)
			break;

                /* Compares lines */
                if(strcmp(newStr1, newStr2)) {
                        comparison = true;
        		if(*limbo != NULL) {
		        	*context = addToList(*limbo, *context);

		        	*limbo = NULL;
        		}

                        gUnmatchingLines++;
			j++;

                        *diff1 = addToList(newStr1, *diff1);
                        *diff2 = addToList(newStr2, *diff2);
                }
                else {
			if(*limbo != NULL) {
				free(*limbo);

				*limbo = NULL;
			}

                        if(gContextLines != 0 && !comparison) {
                                *context = addToContextList(newStr1, *context);

				free(newStr2);
                        }
                        else if(gContextLines != 0 && comparison) {
                                *limbo = (char *)calloc(256, sizeof(char));

                                strcpy(*limbo, newStr1);

				free(newStr1);
				free(newStr2);
                        }
			else {
				free(newStr1);
				free(newStr2);
			}

                        break;
                }

                if(j >= gMaxLines)
                        break;
        }

        return comparison;
}

void printDifferences(struct list *diff1, struct list *diff2, size_t fromLine, size_t toLine,
                struct list *contextListHead) {
        struct list *listEntry;
        size_t i;

        if(toLine == fromLine)
                printf("differences found from line %lu:\n", fromLine);
        else
                printf("differences found from line %lu to %lu:\n", (fromLine), toLine);

        if(gContextLines != 0 && contextListHead != NULL) {
                listEntry = contextListHead;

                for(i = 0; i < lenList(contextListHead); i++) {
                        printf("C %s\n", listEntry->str);

                        listEntry = listEntry->next;
                }
        	
		printf("\n");
        }

        listEntry = diff1;

        for(i = 0; i < lenList(diff1); i++) {
                printf("< %s\n", listEntry->str);

                listEntry = listEntry->next;
        }

        i = 0;

        printf("\n");

        listEntry = diff2;

        for(i = 0; i < lenList(diff2); i++) {
                printf("> %s\n", listEntry->str);

                listEntry = listEntry->next;
        }

        printf("\n");
}
