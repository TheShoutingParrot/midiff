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

                *(newString+i++) = ch;

                if(ch == '\n')
                        break;
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
        size_t i, a;
        bool comparison;
        char *newStr1,
             *newStr2;

        comparison = false;

        gIsLastLine = false;

        i = 0;

        if(*limbo != NULL) {
                *context = addToList(*limbo, *context);

                *limbo = NULL;

                return false;
        }

        for(;;) {
                newStr1 = (char *)malloc(512);
                newStr2 = (char *)malloc(512);

                /* Checks if the line contained an EOF (end of file) */
                gIsLastLine = getNextString(file1, newStr1) || getNextString(file2, newStr2);

                (*lines)++;
                i++;

                /* Compares lines */
                if(strcmp(newStr1, newStr2)) {
                        comparison = true;

                        gUnmatchingLines++;

                        *diff1 = addToList(newStr1, *diff1);
                        *diff2 = addToList(newStr2, *diff2);
                }
                else {
                        if(gContextLines != 0 && !comparison) {
                                *context = addToContextList(newStr1, *context);
                        }
                        else if(gContextLines != 0 && comparison) {
                                *limbo = (char *)malloc(strlen(newStr1));

                                strcpy(*limbo, newStr1);
                        }

                        break;
                }

                if(gIsLastLine || i > 100)
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
                        printf("C %s", listEntry->str);

                        listEntry = listEntry->next;
                }
        	
		printf("\n");
        }

        listEntry = diff1;

        for(i = 0; i < lenList(diff1); i++) {
                printf("< %s", listEntry->str);

                listEntry = listEntry->next;
        }

        i = 0;

        printf("\n");

        listEntry = diff2;

        for(i = 0; i < lenList(diff2); i++) {
                printf("> %s", listEntry->str);

                listEntry = listEntry->next;
        }

        printf("\n");
}
