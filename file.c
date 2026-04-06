/*
 * file.c — Implementasi operasi open / save / close
 *
 * Representasi fisik : Alokasi Sequential (Array 2D via TextBuffer)
 *
 * Isi file dibaca baris per baris dan disimpan ke array 2D statis.
 * Setiap baris file → satu baris dalam text[row].
 * Urutan baris diwakili oleh indeks fisik array (posisi sequential).
 *
 * Algoritma Open:
 *   row ← 0
 *   while masih ada baris di file dan row < MAX_ROW:
 *     baca satu baris ke buffer sementara
 *     strip newline (\r\n atau \n)
 *     salin ke text[row], update lineLength[row]
 *     row ← row + 1
 *   totalLines ← row (minimal 1)
 *
 * Algoritma Save:
 *   untuk setiap row dari 0 sampai totalLines - 1:
 *     tulis text[row] ke file
 *     jika bukan baris terakhir: tulis '\n'
 */

#include "file.h"
#include <stdio.h>
#include <string.h>

/*
 * file_open — Buka file dan muat ke buffer (array 2D)
 */
int file_open(TextBuffer *buf, const char *filename) {
    FILE *fp;
    char  barisTemp[MAX_COL + 4];
    int   row = 0;
    int   len;

    fp = fopen(filename, "r");
    if (!fp) return 0;

    buffer_init(buf);

    /* Baca file baris per baris ke array sequential */
    while (fgets(barisTemp, (int)sizeof(barisTemp), fp) && row < MAX_ROW) {
        len = (int)strlen(barisTemp);

        /* Strip newline */
        if (len > 0 && barisTemp[len - 1] == '\n') barisTemp[--len] = '\0';
        if (len > 0 && barisTemp[len - 1] == '\r') barisTemp[--len] = '\0';

        if (len > MAX_COL - 1) len = MAX_COL - 1;

        memcpy(buf->text[row], barisTemp, len);
        buf->text[row][len] = '\0';
        buf->lineLength[row] = len;
        row++;
    }

    buf->totalLines  = (row == 0) ? 1 : row;
    buf->currentRow  = 0;
    fclose(fp);
    return 1;
}

/*
 * file_save — Simpan buffer (array 2D) ke file
 */
int file_save(const TextBuffer *buf, const char *filename) {
    FILE *fp;
    int   row;

    fp = fopen(filename, "w");
    if (!fp) return 0;

    for (row = 0; row < buf->totalLines; row++) {
        fputs(buf->text[row], fp);
        if (row < buf->totalLines - 1) fputc('\n', fp);
    }

    fclose(fp);
    return 1;
}

/*
 * file_close — Reset buffer ke kondisi awal (kosong)
 */
void file_close(TextBuffer *buf) {
    buffer_init(buf);
}
