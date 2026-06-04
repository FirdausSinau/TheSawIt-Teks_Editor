#include "buffer.h"
#include <string.h>
#include <stdio.h>

Node *getNode(TextBuffer *buf, int n) {
    Node *pointer = buf->head;
    int i;

    for (i = 0; i < n; i++) {
        if (pointer == NULL) return NULL;
        pointer = pointer->next;
    }

    return pointer;
}

void bufferGoto(TextBuffer *buf, int nomor) {
    if (nomor < 1) nomor = 1;
    if (nomor > buf->totalLines) nomor = buf->totalLines;
    buf->currentRow = nomor - 1;
}

void bufferInsert(TextBuffer *buf, const char *teks) {
    Node *node = getNode(buf, buf->currentRow);
    int len = (int)strlen(teks);
    int i;

    if (node == NULL) {
        if (buf->head == NULL) {
            bufferInit(buf);
        }
        node = buf->head;
        buf->currentRow = 0;
    }

    for (i = 0; i < len; i++) {
        if (node->length >= MAX_COL - 1) break;
        node->text[node->length] = teks[i];
        node->length++;
    }
    node->text[node->length] = '\0';
}

void bufferBackspace(TextBuffer *buf, int n) {
    Node *node = getNode(buf, buf->currentRow);
    int i;

    if (node == NULL) {
        if (buf->head == NULL) {
            bufferInit(buf);
        }
        node = buf->head;
        buf->currentRow = 0;
    }

    for (i = 0; i < n; i++) {
        if (node->length == 0) break;
        node->length--;
        node->text[node->length] = '\0';
    }
}