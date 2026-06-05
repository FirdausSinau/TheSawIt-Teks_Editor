#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "buffer.h"
#include "file.h"
#include "replace.h"
#include "display.h"

#define CMD_MAX 512

TextBuffer buf;
Stack      undoStack;
Stack      redoStack;
char       namaFile[256];
int        modified;

int tanyaKonfirmasi(char *pertanyaan) {
    char jawab[8];
    printf("%s (y/n): ", pertanyaan);
    fflush(stdout);
    if (!fgets(jawab, sizeof(jawab), stdin)) return 0;
    return (jawab[0] == 'y' || jawab[0] == 'Y');
}

void bersihkanNewline(char *str) {
    int len = (int)strlen(str);
    if (len > 0 && str[len-1] == '\n') str[--len] = '\0';
    if (len > 0 && str[len-1] == '\r') str[--len] = '\0';
}


void cmdInsert(char *teks) {
    int row;
    Node *node;

    if (teks[0] == '\0') {
        printf("[INFO] Ketik teks setelah perintah i. Contoh: i Halo\n");
        return;
    }

    row = buf.currentRow;
    bufferPushUndo(&undoStack, &redoStack, &buf);

    node = getNode(&buf, buf.currentRow);
    if (node != NULL && node->length > 0) {
        bufferInsert(&buf, " ");
    }
    bufferInsert(&buf, teks);

    modified = 1;
    printf("[OK] Baris %d diperbarui.\n", row + 1);
    displayBuffer(&buf, namaFile, modified);
}


void cmdInsertBaris(char *teks) {
    bufferPushUndo(&undoStack, &redoStack, &buf);
    bufferInsertBaris(&buf, teks);
    modified = 1;
    printf("[OK] Baris baru dibuat di baris %d.\n", buf.currentRow + 1);
    displayBuffer(&buf, namaFile, modified);
}


void cmdHapusKarakter(char *argumen) {
    int jumlah = 1;

    if (argumen[0] != '\0') {
        jumlah = atoi(argumen);
        if (jumlah < 1) {
            printf("[INFO] Jumlah tidak valid, menggunakan default 1.\n");
            jumlah = 1;
        }
    }

    bufferPushUndo(&undoStack, &redoStack, &buf);
    bufferBackspace(&buf, jumlah);
    modified = 1;
    printf("[OK] %d karakter dihapus dari baris %d.\n",
           jumlah, buf.currentRow + 1);
    displayBuffer(&buf, namaFile, modified);
}


void cmdHapusBaris(void) {
    int baris_lama = buf.currentRow + 1;
    bufferPushUndo(&undoStack, &redoStack, &buf);
    bufferHapusBaris(&buf);
    modified = 1;
    printf("[OK] Baris %d dihapus.\n", baris_lama);
    displayBuffer(&buf, namaFile, modified);
}


void cmdGoto(char *argumen) {
    int nomor;

    if (argumen[0] == '\0') {
        printf("[ERROR] Masukkan nomor baris. Contoh: g 3\n");
        return;
    }

    nomor = atoi(argumen);
    if (nomor < 1) {
        printf("[ERROR] Nomor baris harus angka positif. Contoh: g 3\n");
        return;
    }

    bufferGoto(&buf, nomor);
    printf("[OK] Pindah ke baris %d.\n", buf.currentRow + 1);
    displayBuffer(&buf, namaFile, modified);
}


void cmdReplace(void) {
    char cari[CMD_MAX];
    char ganti[CMD_MAX];
    int  jumlah;

    printf("  Cari    : ");
    fflush(stdout);
    if (!fgets(cari, sizeof(cari), stdin)) return;
    bersihkanNewline(cari);

    if (cari[0] == '\0') {
        printf("[BATAL] Teks yang dicari tidak boleh kosong.\n");
        return;
    }

    printf("  Ganti   : ");
    fflush(stdout);
    if (!fgets(ganti, sizeof(ganti), stdin)) return;
    bersihkanNewline(ganti);

    bufferPushUndo(&undoStack, &redoStack, &buf);
    jumlah = replaceText(&buf, cari, ganti);

    if (jumlah > 0) {
        modified = 1;
        printf("[OK] %d kemunculan '%s' diganti menjadi '%s'.\n",
               jumlah, cari, ganti);
        displayBuffer(&buf, namaFile, modified);
    } else {
        printf("[INFO] Teks '%s' tidak ditemukan.\n", cari);
    }
}

void cmdBuka(char *argumen) {
    if (argumen[0] == '\0') {
        printf("[ERROR] Masukkan nama file. Contoh: o catatan.txt\n");
        return;
    }

    if (modified) {
        if (!tanyaKonfirmasi("[PERINGATAN] Ada perubahan belum disimpan. Lanjut?"))
        {
            printf("[BATAL] Buka file dibatalkan.\n");
            return;
        }
    }

    if (fileOpen(&buf, argumen)) {
        strncpy(namaFile, argumen, sizeof(namaFile) - 1);
        namaFile[sizeof(namaFile) - 1] = '\0';
        stackInit(&undoStack);
        stackInit(&redoStack);
        modified = 0;
        printf("[OK] File '%s' dibuka (%d baris).\n", namaFile, buf.totalLines);
        displayBuffer(&buf, namaFile, modified);
    } else {
        printf("[ERROR] File '%s' tidak ditemukan atau tidak bisa dibuka.\n",
               argumen);
    }
}

void cmdSimpan(char *argumen) {
    if (argumen[0] != '\0') {
        strncpy(namaFile, argumen, sizeof(namaFile) - 1);
        namaFile[sizeof(namaFile) - 1] = '\0';
    }

    if (namaFile[0] == '\0') {
        printf("[ERROR] Belum ada nama file. Gunakan: s <namafile>\n");
        return;
    }

    if (fileSave(&buf, namaFile)) {
        modified = 0;
        printf("[OK] Disimpan ke '%s'.\n", namaFile);
    } else {
        printf("[ERROR] Gagal menyimpan '%s'.\n", namaFile);
    }
}

void cmdTutup(void) {
    if (modified) {
        if (!tanyaKonfirmasi("[PERINGATAN] Ada perubahan belum disimpan. Lanjut?"))
        {
            printf("[BATAL] Tutup file dibatalkan.\n");
            return;
        }
    }

    fileClose(&buf);
    bufferInit(&buf);
    namaFile[0] = '\0';
    stackInit(&undoStack);
    stackInit(&redoStack);
    modified = 0;
    printf("[OK] Buffer dikosongkan.\n");
    displayBuffer(&buf, namaFile, modified);
}

void cmdHapusFile(char *argumen) {
    if (argumen[0] == '\0') {
        printf("[ERROR] Masukkan nama file. Contoh: del catatan.txt\n");
        return;
    }

    printf("[PERINGATAN] '%s' akan dihapus PERMANEN dari disk.\n", argumen);
        if (!tanyaKonfirmasi("Yakin?")) {
        printf("[BATAL] Hapus file dibatalkan.\n");
        return;
    }

    if (remove(argumen) == 0) {
        printf("[OK] File '%s' dihapus dari disk.\n", argumen);

        /* Jika file yang dihapus adalah yang sedang dibuka, reset namaFile */
        if (strcmp(argumen, namaFile) == 0) {
            namaFile[0] = '\0';
            modified    = 0;
            printf("[INFO] Buffer masih ada di memori. Simpan dengan nama baru jika perlu.\n");
        }
    } else {
        printf("[ERROR] Gagal menghapus '%s'. File tidak ada atau tidak ada izin.\n",
               argumen);
    }
}

void cmdUndo(void) {
    if (bufferUndo(&undoStack, &redoStack, &buf)) {
        modified = 1;
        printf("[OK] Undo berhasil.\n");
        displayBuffer(&buf, namaFile, modified);
    } else {
        printf("[INFO] Tidak ada yang bisa di-undo.\n");
    }
}

void cmdRedo(void) {
    if (bufferRedo(&undoStack, &redoStack, &buf)) {
        modified = 1;
        printf("[OK] Redo berhasil.\n");
        displayBuffer(&buf, namaFile, modified);
    } else {
        printf("[INFO] Tidak ada yang bisa di-redo.\n");
    }
}

void cmdKeluar(void) {
    if (modified) {
        if (!tanyaKonfirmasi("[PERINGATAN] Ada perubahan belum disimpan. Keluar?"))
        {
            printf("[BATAL] Keluar dibatalkan.\n");
            return;
        }
    }
    printf("Sampai jumpa!\n");
    exit(0);
}

void prosesPerintah(char *input) {
    char perintah[16];
    char argumen[CMD_MAX];

    argumen[0]  = '\0';
    perintah[0] = '\0';

    sscanf(input, "%15s %511[^\n]", perintah, argumen);
    if (perintah[0] == '\0') return;

    if      (strcmp(perintah, "i")   == 0) cmdInsert(argumen);
    else if (strcmp(perintah, "ia")  == 0) cmdInsertBaris(argumen);
    else if (strcmp(perintah, "d")   == 0) cmdHapusKarakter(argumen);
    else if (strcmp(perintah, "dl")  == 0) cmdHapusBaris();
    else if (strcmp(perintah, "g")   == 0) cmdGoto(argumen);
    else if (strcmp(perintah, "f")   == 0) cmdReplace();
    else if (strcmp(perintah, "o")   == 0) cmdBuka(argumen);
    else if (strcmp(perintah, "s")   == 0) cmdSimpan(argumen);
    else if (strcmp(perintah, "w")   == 0) cmdTutup();
    else if (strcmp(perintah, "del") == 0) cmdHapusFile(argumen);
    else if (strcmp(perintah, "u")   == 0) cmdUndo();
    else if (strcmp(perintah, "r")   == 0) cmdRedo();
    else if (strcmp(perintah, "h")   == 0) displayBantuan();
    else if (strcmp(perintah, "q")   == 0) cmdKeluar();
    else {
        printf("[ERROR] Perintah '%s' tidak dikenal. Ketik h untuk bantuan.\n",
               perintah);
    }
}

int main(int argc, char *argv[]) {
    char cmd[CMD_MAX];
    int  len;

    bufferInit(&buf);
    stackInit(&undoStack);
    stackInit(&redoStack);
    namaFile[0] = '\0';
    modified    = 0;

    printf("\n=== Text Editor CLI  |  Linked List + Stack ===\n\n");

    displayBantuan();

    if (argc > 1) {
        if (fileOpen(&buf, argv[1])) {
            strncpy(namaFile, argv[1], sizeof(namaFile) - 1);
            namaFile[sizeof(namaFile) - 1] = '\0';
            printf("[OK] File '%s' dibuka (%d baris).\n",
                   namaFile, buf.totalLines);
        } else {
            strncpy(namaFile, argv[1], sizeof(namaFile) - 1);
            namaFile[sizeof(namaFile) - 1] = '\0';
            printf("[INFO] File '%s' belum ada, dimulai kosong.\n", argv[1]);
        }
    }

    displayBuffer(&buf, namaFile, modified);

    while (1) {
        printf("[%s%s | brs %d/%d]> ",
               namaFile[0] ? namaFile : "baru",
               modified     ? "*"     : "",
               buf.currentRow + 1,
               buf.totalLines);
        fflush(stdout);

        if (!fgets(cmd, sizeof(cmd), stdin)) break;

        len = (int)strlen(cmd);
        if (len > 0 && cmd[len-1] == '\n') cmd[--len] = '\0';
        if (len > 0 && cmd[len-1] == '\r') cmd[--len] = '\0';

        if (len == 0) continue;

        prosesPerintah(cmd);
    }

    return 0;
}