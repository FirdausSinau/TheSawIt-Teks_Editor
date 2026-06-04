#include "display.h"
#include <stdio.h>
#include <string.h>

void displayBuffer(TextBuffer *buf, char *namaFile, int modified){
    Node *cur; 
    int i; 
    char penanda; 
    char *tampilNama; 
    char *tampilModified; 

    if (namaFile[0] != NULL){ 
        tampilNama = namaFile; 
    } else { 
        tampilNama = "(belum disimpan)"; 
    }

    if (modified == 1){ 
        tampilModified = " [*]"; 
    } else if (modified == 0){ 
        tampilModified = ""; 
    }

    printf("\n=== File: %s%s  |  Baris: %d/%d ===\n", tampilNama, tampilModified, buf->currentRow + 1, buf->totalLines);

    cur = buf->head; 
    i = 0; 

    while(cur != NULL){ 
        if(i == buf->currentRow){ 
            penanda = ">"; 
        } else{ 
            penanda = " "; 
        }

        printf("%c%3d : %s\n", penanda, i + 1, cur->text); 

        cur = cur->next; 
        i++; 
    }

    printf("===\n\n"); 
}

}

void displayBantuan(void) {
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