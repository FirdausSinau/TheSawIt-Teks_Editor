#include "buffer.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

Node *allocNode(void) {
    Node *node = malloc(sizeof(Node));
    if (node == NULL) {
        printf("[ERROR] Memori penuh.\n");
        return NULL;
    }
    node->text[0] = '\0';
    node->length = 0;
    node->next = NULL;
    return node;
}

void freeNode(Node *node) {
    if (node == NULL) return;
    free(node);
}

void bufferInsertBaris(TextBuffer *buf, char *teks) {
    Node *baru = allocNode();
    if (baru == NULL) return;

    strncpy(baru->text, teks, MAX_COL - 1);
    baru->text[MAX_COL - 1] = '\0';
    baru->length = strlen(teks);

    Node *current = getNode(buf, buf->currentRow);

    baru->next    = current->next;
    current->next = baru;

    buf->currentRow++;
    buf->totalLines++;
}

void bufferHapusBaris(TextBuffer *buf) {
    if (buf->totalLines == 1) {
        Node *satu = buf->head;
        satu->text[0] = '\0';
        satu->length  = 0;
        return;
    }

    if (buf->currentRow == 0) {
        Node *hapus  = buf->head;
        buf->head    = hapus->next;
        freeNode(hapus);
        buf->totalLines--;
        return;
    }

    Node *prev  = getNode(buf, buf->currentRow - 1);
    Node *hapus = prev->next;

    prev->next = hapus->next;

    if (hapus->next == NULL) {
        buf->currentRow--;
    }

    freeNode(hapus);
    buf->totalLines--;
}