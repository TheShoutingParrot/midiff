#include "midiff.h"

struct list *addToList(char *str, struct list *listHead) {
        struct list *listEntry;

        if(listHead == NULL) {
                listHead = (struct list *)malloc(sizeof(*listHead));

                listEntry = listHead;
        }

        else {
                listEntry = listHead;

                while(listEntry->next != NULL) {
                        listEntry = listEntry->next;
                }

                listEntry->next = (struct list *)malloc(sizeof(*listHead));
                listEntry = listEntry->next;
        }

        listEntry->str = str;
        listEntry->next = NULL;

        return listHead;
}

struct list *addToContextList(char *str, struct list *listHead) {
        struct list *listEntry;

        if(lenList(listHead) >= gContextLines) {
                listEntry = listHead->next;

                free(listHead->str);
                free(listHead);

                listHead = listEntry;
        }

        return addToList(str, listHead);
}

size_t lenList(struct list *listEntry) {
        size_t i;

        i = 0;

        while(listEntry != NULL) {
                listEntry = listEntry->next;
                i++;
        }

        return i;
}

#ifdef DEBUG
void printList(struct list *listHead) {
        struct list *listEntry;
        size_t i;

        listEntry = listHead;

        i = 0;

        while(listEntry != NULL) {
                printf("listEntry %p str: %p %s\n",
				listEntry, listEntry->str, listEntry->str);
                listEntry = listEntry->next;
                i++;
        }
}
#endif /* #ifdef DEBUG */

void deleteList(struct list *listHead) {
        struct list *listEntry, *temp;

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
