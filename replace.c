#include "replace.h"
#include <string.h>

int replaceText(TextBuffer *buf, char *cari, char *ganti) {
    int cariLen  = (int)strlen(cari);
    int gantiLen = (int)strlen(ganti);
    int count    = 0;
    Node *node;

    if (cariLen == 0) return 0;

    node = buf->head;
    while (node != NULL) {
        char temp[MAX_COL];
        int  tempLen = 0;
        int  i = 0;
        int  rowLen = node->length;
        int  replaced = 0;

        while (i < rowLen) {
            if (i <= rowLen - cariLen &&
                memcmp(node->text + i, cari, (size_t)cariLen) == 0) {

                if (tempLen + gantiLen < MAX_COL - 1) {
                    memcpy(temp + tempLen, ganti, (size_t)gantiLen);
                    tempLen += gantiLen;
                }
                i += cariLen;
                count++;
                replaced = 1;
            } else {
                if (tempLen < MAX_COL - 1)
                    temp[tempLen++] = node->text[i];
                i++;
            }
        }

        if (replaced) {
            temp[tempLen] = '\0';
            memcpy(node->text, temp, (size_t)(tempLen + 1));
            node->length = tempLen;
        }

        node = node->next;
    }

    return count;
}
