#include "replace.h"
#include <string.h>

int replace_text(TextBuffer *buf, const char *cari, const char *ganti) {
    int cariLen  = (int)strlen(cari);
    int gantiLen = (int)strlen(ganti);
    int count    = 0;
    int r;

    if (cariLen == 0) return 0;

    for (r = 0; r < buf->totalLines; r++) {
        char baruLine[MAX_COL];
        int  baruLen = 0;
        int  i       = 0;
        int  rowLen  = buf->lineLength[r];  /* cache biar ga akses struct tiap iterasi */

        while (i < rowLen) {
            if (i <= rowLen - cariLen &&
                memcmp(buf->text[r] + i, cari, (size_t)cariLen) == 0) {

                if (baruLen + gantiLen < MAX_COL - 1) {
                    memcpy(baruLine + baruLen, ganti, (size_t)gantiLen);
                    baruLen += gantiLen;
                }
                i += cariLen;
                count++;
            } else {
                if (baruLen < MAX_COL - 1)
                    baruLine[baruLen++] = buf->text[r][i];
                i++;
            }
        }

        baruLine[baruLen] = '\0';
        memcpy(buf->text[r], baruLine, (size_t)(baruLen + 1));
        buf->lineLength[r] = baruLen;
    }

    return count;
}