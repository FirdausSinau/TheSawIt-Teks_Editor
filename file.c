#include "file.h"
#include <stdio.h>
#include <string.h>


int fileOpen(TextBuffer *buf, const char *filename) {
    FILE *fp;
    char  barisTemp[MAX_COL + 4];
    int firstLine = 1;
    int   len;

    fp = fopen(filename, "r");
    if (!fp) return 0;

    bufferInit(buf);

    while (fgets(barisTemp, (int)sizeof(barisTemp), fp)) {
        len = (int)strlen(barisTemp);
        if (len > 0 && barisTemp[len - 1] == '\n') barisTemp[--len] = '\0';
        if (len > 0 && barisTemp[len - 1] == '\r') barisTemp[--len] = '\0';
        
		if (len > MAX_COL - 1) {
			len = MAX_COL - 1;
			barisTemp[len] = '\0';
		}
        		
		if (firstLine) {
        	bufferInsert(buf, barisTemp);
            firstLine = 0;
        } else {
            bufferInsertBaris(buf, barisTemp);
        }
		
    }
	bufferGoto(buf, 1);
	
    fclose(fp);
    return 1;
}


int fileSave(const TextBuffer *buf, const char *filename) {
    FILE *fp;
    Node *node;
    int   row;

    fp = fopen(filename, "w");
    if (!fp) return 0;

	node = buf->head;
    while (node != NULL) {
    fputs(node->text, fp);
    if (node->next != NULL) fputc('\n', fp);
    node = node->next;
	}
	
    fclose(fp);
    return 1;
}


void fileClose(TextBuffer *buf) {
	Node *cur = buf->head;
    while (cur != NULL) {
        Node *next = cur->next;
        free(cur);
        cur = next;
    }
    bufferInit(buf);
}