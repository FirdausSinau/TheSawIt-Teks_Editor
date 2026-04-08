#include "display.h"
#include <stdio.h>
#include <string.h>

void display_buffer(const TextBuffer *buf,
                    const char       *namaFile,
                    int               modified) {
    int i;

    printf("\n=== File: %s%s  |  Baris: %d/%d ===\n",
           namaFile[0] ? namaFile : "(belum disimpan)",
           modified     ? " [*]"  : "",
           buf->currentRow + 1,
           buf->totalLines);

    for (i = 0; i < buf->totalLines; i++) {
        printf("%c%3d : %s\n",
               (i == buf->currentRow) ? '>' : ' ',
               i + 1,
               buf->text[i]);
    }

    printf("===\n\n");
}

void display_bantuan(void) {
    puts(
        "\n+-------------+------------------------------------------+\n"
        "| Perintah    | Keterangan                               |\n"
        "+-------------+------------------------------------------+\n"
        "| i <teks>    | Tambah teks di akhir baris aktif         |\n"
        "| ia <teks>   | Buat baris baru di bawah baris aktif     |\n"
        "| d [n]       | Hapus n karakter terakhir (def: 1)       |\n"
        "| dl          | Hapus seluruh baris aktif                |\n"
        "| g <nomor>   | Pindah ke baris nomor tertentu           |\n"
        "| f           | Cari dan ganti teks (2 prompt)           |\n"
        "| o <file>    | Buka file dari disk                      |\n"
        "| s [file]    | Simpan file ke disk                      |\n"
        "| w           | Tutup file / reset buffer                |\n"
        "| del <file>  | Hapus file dari disk (permanen)          |\n"
        "| u           | Undo - batalkan perubahan terakhir       |\n"
        "| r           | Redo - ulangi yang di-undo               |\n"
        "| h           | Tampilkan bantuan ini                    |\n"
        "| q           | Keluar dari program                      |\n"
        "+-------------+------------------------------------------+\n"
        "| Catatan: [*] = ada perubahan belum disimpan            |\n"
        "|          >   = baris yang sedang aktif                 |\n"
        "+--------------------------------------------------------+"
    );
    putchar('\n');
}