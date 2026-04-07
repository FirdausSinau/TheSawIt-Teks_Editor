/*
 * display.c - Implementasi tampilan buffer dan bantuan
 *
 * Format tampilan dirancang sederhana:
 *   - Tidak ada garis kotak (tidak pakai +--+)
 *   - Pemisah menggunakan ':' antara nomor baris dan isi teks
 *   - Baris aktif ditandai '>' di depan nomor
 *   - Header dan footer menggunakan '==='
 *
 * Contoh output:
 *
 *   === File: catatan.txt [*]  |  Baris: 2/3 ===
 *    1 : Halo dunia
 *   >2 : Baris aktif ini
 *    3 : Baris ketiga
 *   ===
 */

#include "display.h"
#include <stdio.h>
#include <string.h>

/*
 * display_buffer - Tampilkan seluruh isi buffer
 *
 * Algoritma:
 *   1. Cetak header berisi nama file, status, dan posisi baris
 *   2. Loop i dari 0 sampai totalLines - 1:
 *      - Jika i == currentRow: cetak '>' sebagai tanda aktif
 *      - Cetak nomor baris (i+1), lalu ':', lalu isi teks
 *   3. Cetak footer '==='
 */
void display_buffer(const TextBuffer *buf,
                    const char       *namaFile,
                    int               modified) {
    int i;

    printf("\n");

    /* Header: info file dan posisi */
    printf("=== File: %s%s  |  Baris: %d/%d ===\n",
           namaFile[0] ? namaFile : "(belum disimpan)",
           modified     ? " [*]"  : "",
           buf->currentRow + 1,
           buf->totalLines);

    /* Isi baris satu per satu */
    for (i = 0; i < buf->totalLines; i++) {
        /*
         * Format tiap baris:
         *   ">NNN : isi teks"  -> baris aktif (ada '>')
         *   " NNN : isi teks"  -> baris biasa (ada spasi)
         *
         * %3d  -> nomor baris rata kanan, lebar 3 digit
         * ' '  -> spasi setelah nomor sebelum ':'
         */
        if (i == buf->currentRow)
            printf(">%3d : %s\n", i + 1, buf->text[i]);
        else
            printf(" %3d : %s\n", i + 1, buf->text[i]);
    }

    /* Footer */
    printf("===\n\n");
}

/*
 * display_bantuan - Tampilkan daftar perintah
 *
 * Format sederhana: perintah diikuti penjelasan singkat.
 * Tidak ada garis kotak, cukup tanda '---' sebagai pemisah.
 */
void display_bantuan(void) {
    printf("\n");
    printf("=== DAFTAR PERINTAH ===\n");
    printf("  i <teks>    : Tambah teks di akhir baris aktif\n");
    printf("  ia <teks>   : Buat baris baru di bawah baris aktif\n");
    printf("  d [n]       : Hapus n karakter terakhir (default: 1)\n");
    printf("  dl          : Hapus seluruh baris aktif\n");
    printf("  g <nomor>   : Pindah ke baris nomor tertentu\n");
    printf("  f           : Cari dan ganti teks (2 prompt)\n");
    printf("  o <file>    : Buka file dari disk\n");
    printf("  s [file]    : Simpan file ke disk\n");
    printf("  w           : Tutup file / reset buffer\n");
    printf("  del <file>  : Hapus file dari disk (permanen)\n");
    printf("  u           : Undo - batalkan perubahan terakhir\n");
    printf("  r           : Redo - ulangi yang di-undo\n");
    printf("  h           : Tampilkan bantuan ini\n");
    printf("  q           : Keluar dari program\n");
    printf("---\n");
    printf("  [*] = ada perubahan belum disimpan\n");
    printf("   >  = baris yang sedang aktif\n");
    printf("===\n\n");
}
