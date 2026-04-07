#ifndef DISPLAY_H
#define DISPLAY_H

/*
 * display.h - Modul tampilan buffer dan bantuan
 *
 * Memisahkan semua logika tampilan dari main.c agar main.c
 * hanya berisi: init, main loop, dan parse perintah.
 *
 * Format tampilan (sederhana, tanpa garis kotak):
 *
 *   === File: catatan.txt [*]  |  Baris: 2/3 ===
 *    1 : Halo dunia
 *   >2 : Baris aktif ini
 *    3 : Baris ketiga
 *   ===
 *
 * Keterangan:
 *   >  = baris aktif
 *   [*] = ada perubahan belum disimpan
 */

#include "buffer.h"

/*
 * display_buffer - Tampilkan isi buffer ke layar
 *
 * Parameter:
 *   buf      - pointer ke TextBuffer yang akan ditampilkan
 *   namaFile - nama file aktif (string kosong jika belum ada)
 *   modified - 1 jika ada perubahan belum disimpan
 */
void display_buffer(const TextBuffer *buf,
                    const char       *namaFile,
                    int               modified);

/*
 * display_bantuan - Tampilkan daftar semua perintah
 */
void display_bantuan(void);

#endif /* DISPLAY_H */
