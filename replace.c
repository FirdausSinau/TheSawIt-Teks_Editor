/*
 * replace.c — Implementasi find & replace
 *
 * Representasi fisik : Alokasi Sequential (Array 2D via TextBuffer)
 *
 * Pencarian dilakukan LINEAR: baris per baris, posisi per posisi.
 * Sifat ini langsung mencerminkan alokasi sequential di materi —
 * urutan elemen diwakili oleh posisi fisik dalam array.
 *
 * Algoritma replace_text():
 *   untuk setiap baris r dari 0 sampai totalLines - 1:
 *     i ← 0
 *     while i < lineLength[r]:
 *       cek apakah 'cari' cocok mulai di posisi i
 *         (bandingkan karakter satu per satu)
 *       jika cocok:
 *         salin 'ganti' ke baruLine
 *         i ← i + panjang(cari)   → lompat melewati string yang diganti
 *         count++
 *       jika tidak:
 *         salin 1 karakter ke baruLine
 *         i ← i + 1
 *     update text[r] dengan baruLine
 */

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

            /* Cek kecocokan 'cari' mulai di posisi i */
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
                /* Salin string 'ganti' ke baris baru */
                if (baruLen + gantiLen < MAX_COL - 1) {
                    memcpy(baruLine + baruLen, ganti, gantiLen);
                    baruLen += gantiLen;
                }
                i += cariLen;
                count++;
            } else {
                /* Salin satu karakter biasa */
                if (baruLen < MAX_COL - 1)
                    baruLine[baruLen++] = buf->text[r][i];
                i++;
            }
        }

        baruLine[baruLen] = '\0';

        /* Update baris di array 2D */
        memcpy(buf->text[r], baruLine, baruLen + 1);
        buf->lineLength[r] = baruLen;
    }

    return count;
}
