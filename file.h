#ifndef FILE_H
#define FILE_H

/*
 * file.h — Operasi open / save / close file
 *
 * Semua operasi membaca/menulis TextBuffer (array 2D)
 * dari/ke file di disk, baris per baris.
 */

#include "buffer.h"

/* Buka file dan muat ke buffer. Return 1 sukses, 0 gagal. */
int  file_open(TextBuffer *buf, const char *filename);

/* Simpan buffer ke file. Return 1 sukses, 0 gagal. */
int  file_save(const TextBuffer *buf, const char *filename);

/* Reset buffer ke kondisi awal (file baru / close). */
void file_close(TextBuffer *buf);

#endif /* FILE_H */
