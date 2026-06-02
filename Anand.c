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