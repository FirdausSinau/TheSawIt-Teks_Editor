#include "display.h"
#include <stdio.h>
#include <string.h>

void display_buffer(const TextBuffer *buf,
                    const char       *namaFile,
                    int               modified) {
    int i;

    printf("\n");

    printf("=== File: %s%s  |  Baris: %d/%d ===\n",
           namaFile[0] ? namaFile : "(belum disimpan)",
           modified     ? " [*]"  : "",
           buf->currentRow + 1,
           buf->totalLines);

    for (i = 0; i < buf->totalLines; i++) {
        if (i == buf->currentRow)
            printf(">%3d : %s\n", i + 1, buf->text[i]);
        else
            printf(" %3d : %s\n", i + 1, buf->text[i]);
    }

    printf("===\n\n");
}

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