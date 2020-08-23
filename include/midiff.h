#ifndef _MIDIFF_H
#define _MIDIFF_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>

#define VERSION         "0.1.0"

/* struct used for list of differences and list of
 * "context" lines */
struct list {
        char *str;
        struct list *next;
};

bool gIsLastLine;
size_t gUnmatchingLines,
       gContextLines;

/* compare.c functions */
bool getNextString(FILE *file, char *newString);
bool compareLines(FILE *file1, FILE *file2,
		size_t *lines,
		struct list **diff1, struct list **diff2, struct list **listHead,
                char **limbo);
void printDifferences(struct list *diff1, struct list *diff2,
                size_t fromLine, size_t toLine, struct list *contextListHead);

/* list.c functions */
struct list *addToContextList(char *str, struct list *listHead);
struct list *addToList(char *str, struct list *listHead);
size_t lenList(struct list *listEntry);
void printList(struct list *listHead);
void deleteList(struct list *listHead);

/* util.c functions */
void usage(void);
void die(const char *fmt, ...);

#endif /* #ifndef _MIDIFF_H */
