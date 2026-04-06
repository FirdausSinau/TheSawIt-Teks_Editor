#ifndef REPLACE_H
#define REPLACE_H

/*
 * replace.h — Operasi find & replace
 *
 * Pencarian dilakukan secara linear pada array 2D,
 * baris per baris, karakter per karakter.
 */

#include "buffer.h"

/*
 * replace_text — Ganti semua kemunculan 'cari' dengan 'ganti'
 * Return: jumlah penggantian yang dilakukan.
 */
int replace_text(TextBuffer *buf, const char *cari, const char *ganti);

#endif /* REPLACE_H */
