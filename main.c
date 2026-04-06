/*
 * main.c — Text Editor CLI berbasis Command
 *
 * Tidak ada raw mode, tidak ada shortcut key.
 * User mengetik perintah lalu Enter.
 *
 * Daftar perintah:
 *   i <teks>         → insert teks di akhir baris aktif
 *   ia <teks>        → insert baris baru di bawah baris aktif
 *   d [n]            → hapus n karakter terakhir (default 1)
 *   dl               → hapus seluruh baris aktif
 *   g <nomor>        → pindah ke baris nomor
 *   f <cari>|<ganti> → find & replace (pisah dengan '|')
 *   o <namafile>     → buka file
 *   s [namafile]     → simpan file
 *   w                → tutup file (reset buffer)
 *   u                → undo
 *   r                → redo
 *   t                → tampilkan seluruh isi buffer
 *   h                → tampilkan daftar perintah
 *   q                → keluar
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "buffer.h"
#include "file.h"
#include "replace.h"

#define CMD_MAX 512

/* ===== State Global Editor ===== */

static TextBuffer buf;
static Stack      undoStack;
static Stack      redoStack;
static char       filename[256];
static int        modified;

/* ===== Tampilan ===== */

/*
 * tampil_buffer — Tampilkan isi buffer ke layar
 *
 * Format:
 *   nomor baris | isi baris
 *   Baris aktif ditandai dengan tanda '>'
 */
static void tampil_buffer(void) {
    int i;
    printf("\n");
    printf("================================================\n");
    printf(" File: %s%s\n",
           filename[0] ? filename : "(belum disimpan)",
           modified ? " [*]" : "");
    printf(" Total baris: %d | Baris aktif: %d\n",
           buf.totalLines, buf.currentRow + 1);
    printf("================================================\n");

    for (i = 0; i < buf.totalLines; i++) {
        if (i == buf.currentRow)
            printf("> %3d | %s\n", i + 1, buf.text[i]);
        else
            printf("  %3d | %s\n", i + 1, buf.text[i]);
    }
    printf("================================================\n\n");
}

static void tampil_bantuan(void) {
    printf("\n=== Daftar Perintah ===\n");
    printf("  i <teks>          insert teks di akhir baris aktif\n");
    printf("  ia <teks>         insert baris baru di bawah baris aktif\n");
    printf("  d [n]             hapus n karakter terakhir (default 1)\n");
    printf("  dl                hapus seluruh baris aktif\n");
    printf("  g <nomor>         pindah ke baris nomor\n");
    printf("  f <cari>|<ganti>  find & replace (pisah dengan '|')\n");
    printf("  o <namafile>      buka file\n");
    printf("  s [namafile]      simpan file\n");
    printf("  w                 tutup file (reset buffer)\n");
    printf("  u                 undo\n");
    printf("  r                 redo\n");
    printf("  t                 tampilkan isi buffer\n");
    printf("  h                 tampilkan bantuan ini\n");
    printf("  q                 keluar\n\n");
}

/* ===== Parser Perintah ===== */

static void proses_perintah(char *cmd) {
    char  perintah[16];
    char  argumen[CMD_MAX];
    int   n;

    /* Pisahkan perintah dan argumen */
    argumen[0] = '\0';
    n = sscanf(cmd, "%15s %511[^\n]", perintah, argumen);
    if (n < 1) return;

    /* ===== i — insert teks di baris aktif ===== */
    if (strcmp(perintah, "i") == 0) {
        buffer_push_undo(&undoStack, &redoStack, &buf);
        buffer_insert(&buf, argumen);
        modified = 1;
        printf("[OK] Teks ditambahkan ke baris %d.\n", buf.currentRow + 1);

    /* ===== ia — insert baris baru ===== */
    } else if (strcmp(perintah, "ia") == 0) {
        buffer_push_undo(&undoStack, &redoStack, &buf);
        buffer_insert_baris(&buf, argumen);
        modified = 1;
        printf("[OK] Baris baru dibuat di baris %d.\n", buf.currentRow + 1);

    /* ===== d — hapus karakter terakhir (backspace) ===== */
    } else if (strcmp(perintah, "d") == 0) {
        int jumlah = 1;
        if (argumen[0] != '\0') jumlah = atoi(argumen);
        if (jumlah < 1) jumlah = 1;
        buffer_push_undo(&undoStack, &redoStack, &buf);
        buffer_backspace(&buf, jumlah);
        modified = 1;
        printf("[OK] %d karakter dihapus dari baris %d.\n",
               jumlah, buf.currentRow + 1);

    /* ===== dl — hapus seluruh baris aktif ===== */
    } else if (strcmp(perintah, "dl") == 0) {
        buffer_push_undo(&undoStack, &redoStack, &buf);
        buffer_hapus_baris(&buf);
        modified = 1;
        printf("[OK] Baris dihapus. Sekarang di baris %d.\n",
               buf.currentRow + 1);

    /* ===== g — pindah ke baris ===== */
    } else if (strcmp(perintah, "g") == 0) {
        int nomor = atoi(argumen);
        buffer_goto(&buf, nomor);
        printf("[OK] Pindah ke baris %d.\n", buf.currentRow + 1);

    /* ===== f — find & replace ===== */
    } else if (strcmp(perintah, "f") == 0) {
        char *pemisah = strchr(argumen, '|');
        if (!pemisah) {
            printf("[ERROR] Format: f <cari>|<ganti>\n");
            return;
        }
        *pemisah = '\0'; /* potong argumen di posisi '|' */
        char *cari  = argumen;
        char *ganti = pemisah + 1;

        buffer_push_undo(&undoStack, &redoStack, &buf);
        int jumlah = replace_text(&buf, cari, ganti);
        if (jumlah > 0) {
            modified = 1;
            printf("[OK] %d kemunculan '%s' diganti dengan '%s'.\n",
                   jumlah, cari, ganti);
        } else {
            printf("[INFO] Teks '%s' tidak ditemukan.\n", cari);
        }

    /* ===== o — buka file ===== */
    } else if (strcmp(perintah, "o") == 0) {
        if (modified) {
            printf("[PERINGATAN] Ada perubahan belum disimpan. ");
            printf("Lanjut? (y/n): ");
            char konfirmasi[8];
            if (!fgets(konfirmasi, sizeof(konfirmasi), stdin)) return;
            if (konfirmasi[0] != 'y' && konfirmasi[0] != 'Y') {
                printf("[BATAL] Buka file dibatalkan.\n");
                return;
            }
        }
        if (file_open(&buf, argumen)) {
            strncpy(filename, argumen, sizeof(filename) - 1);
            filename[sizeof(filename) - 1] = '\0';
            stack_init(&undoStack);
            stack_init(&redoStack);
            modified = 0;
            printf("[OK] File '%s' berhasil dibuka (%d baris).\n",
                   filename, buf.totalLines);
        } else {
            printf("[ERROR] Gagal membuka file '%s'.\n", argumen);
        }

    /* ===== s — simpan file ===== */
    } else if (strcmp(perintah, "s") == 0) {
        if (argumen[0] != '\0') {
            strncpy(filename, argumen, sizeof(filename) - 1);
            filename[sizeof(filename) - 1] = '\0';
        }
        if (filename[0] == '\0') {
            printf("[ERROR] Belum ada nama file. Gunakan: s <namafile>\n");
            return;
        }
        if (file_save(&buf, filename)) {
            modified = 0;
            printf("[OK] File berhasil disimpan ke '%s'.\n", filename);
        } else {
            printf("[ERROR] Gagal menyimpan file.\n");
        }

    /* ===== w — tutup file / reset buffer ===== */
    } else if (strcmp(perintah, "w") == 0) {
        if (modified) {
            printf("[PERINGATAN] Ada perubahan belum disimpan. ");
            printf("Lanjut? (y/n): ");
            char konfirmasi[8];
            if (!fgets(konfirmasi, sizeof(konfirmasi), stdin)) return;
            if (konfirmasi[0] != 'y' && konfirmasi[0] != 'Y') {
                printf("[BATAL] Tutup file dibatalkan.\n");
                return;
            }
        }
        file_close(&buf);
        filename[0] = '\0';
        stack_init(&undoStack);
        stack_init(&redoStack);
        modified = 0;
        printf("[OK] Buffer dikosongkan. Editor siap untuk file baru.\n");

    /* ===== u — undo ===== */
    } else if (strcmp(perintah, "u") == 0) {
        if (buffer_undo(&undoStack, &redoStack, &buf)) {
            modified = 1;
            printf("[OK] Undo berhasil. Baris aktif: %d.\n",
                   buf.currentRow + 1);
        } else {
            printf("[INFO] Tidak ada yang bisa di-undo.\n");
        }

    /* ===== r — redo ===== */
    } else if (strcmp(perintah, "r") == 0) {
        if (buffer_redo(&undoStack, &redoStack, &buf)) {
            modified = 1;
            printf("[OK] Redo berhasil. Baris aktif: %d.\n",
                   buf.currentRow + 1);
        } else {
            printf("[INFO] Tidak ada yang bisa di-redo.\n");
        }

    /* ===== t — tampilkan buffer ===== */
    } else if (strcmp(perintah, "t") == 0) {
        tampil_buffer();

    /* ===== h — bantuan ===== */
    } else if (strcmp(perintah, "h") == 0) {
        tampil_bantuan();

    /* ===== q — keluar ===== */
    } else if (strcmp(perintah, "q") == 0) {
        if (modified) {
            printf("[PERINGATAN] Ada perubahan belum disimpan. ");
            printf("Keluar? (y/n): ");
            char konfirmasi[8];
            if (!fgets(konfirmasi, sizeof(konfirmasi), stdin)) return;
            if (konfirmasi[0] != 'y' && konfirmasi[0] != 'Y') {
                printf("[BATAL] Keluar dibatalkan.\n");
                return;
            }
        }
        printf("Sampai jumpa!\n");
        exit(0);

    } else {
        printf("[ERROR] Perintah '%s' tidak dikenal. Ketik 'h' untuk bantuan.\n",
               perintah);
    }
}

/* ===== Main ===== */

int main(int argc, char *argv[]) {
    char cmd[CMD_MAX];

    /* Inisialisasi */
    buffer_init(&buf);
    stack_init(&undoStack);
    stack_init(&redoStack);
    filename[0] = '\0';
    modified    = 0;

    printf("=== Text Editor CLI ===\n");
    printf("Ketik 'h' untuk daftar perintah, 'q' untuk keluar.\n\n");

    /* Buka file dari argumen command line jika ada */
    if (argc > 1) {
        if (file_open(&buf, argv[1])) {
            strncpy(filename, argv[1], sizeof(filename) - 1);
            filename[sizeof(filename) - 1] = '\0';
            printf("[OK] File '%s' dibuka (%d baris).\n\n",
                   filename, buf.totalLines);
        } else {
            printf("[INFO] File '%s' tidak ditemukan, dimulai kosong.\n\n",
                   argv[1]);
            strncpy(filename, argv[1], sizeof(filename) - 1);
        }
    }

    /* Main loop: baca perintah → proses → ulangi */
    while (1) {
        printf("(%s | baris %d/%d)> ",
               filename[0] ? filename : "baru",
               buf.currentRow + 1,
               buf.totalLines);
        fflush(stdout);

        if (!fgets(cmd, sizeof(cmd), stdin)) break;

        /* Hapus newline di akhir */
        int len = (int)strlen(cmd);
        if (len > 0 && cmd[len - 1] == '\n') cmd[--len] = '\0';
        if (len > 0 && cmd[len - 1] == '\r') cmd[--len] = '\0';

        if (len == 0) continue; /* baris kosong, skip */

        proses_perintah(cmd);
    }

    return 0;
}
