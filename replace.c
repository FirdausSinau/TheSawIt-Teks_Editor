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

        while (i < buf->lineLength[r]) {
            int j, cocok = 0;

            if (i <= buf->lineLength[r] - cariLen) {
                cocok = 1;
                for (j = 0; j < cariLen; j++) {
                    if (buf->text[r][i + j] != cari[j]) {
                        cocok = 0;
                        break;
                    }
                }
            }

            if (cocok) {
                if (baruLen + gantiLen < MAX_COL - 1) {
                    memcpy(baruLine + baruLen, ganti, gantiLen);
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

        memcpy(buf->text[r], baruLine, baruLen + 1);
        buf->lineLength[r] = baruLen;
    }

    return count;
}