/*
 * main.c - Text Editor CLI
 *
 * Tanggung jawab file ini HANYA:
 *   1. Inisialisasi semua struktur data
 *   2. Main loop (baca input -> proses -> ulangi)
 *   3. Parsing dan dispatch perintah
 *
 * Semua tampilan -> display.c
 * Semua operasi teks -> buffer.c
 * Semua operasi file -> file.c
 * Semua find/replace -> replace.c
 *
 * -------------------------------------------------------
 * Perintah i - Opsi A:
 *   sscanf memisahkan perintah dan argumen berdasarkan spasi.
 *   Satu spasi pertama adalah pemisah perintah-argumen.
 *   Teks yang masuk adalah PERSIS apa yang ditulis setelah
 *   spasi pertama.
 *
 *   Contoh:
 *     "i halo"      -> argumen = "halo"
 *     "i halo dunia"-> argumen = "halo dunia"
 *
 *   Spasi antar kata di baris aktif ditangani otomatis:
 *   jika baris sudah ada isi, tambah ' ' sebelum teks baru.
 * -------------------------------------------------------
 *
 * Struktur array yang digunakan:
 *   TextBuffer.text[MAX_ROW][MAX_COL] -> array 2D statis
 *   Stack.entries[HISTORY_SIZE]       -> array statis untuk undo/redo
 *   Tidak ada array dinamis, tidak ada pointer array.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "buffer.h"
#include "file.h"
#include "replace.h"
#include "display.h"

#define CMD_MAX 512

/* ============================================================
 * State global editor
 * Hanya 4 variabel: buffer, dua stack, nama file, flag modified
 * ============================================================ */
static TextBuffer buf;
static Stack      undoStack;
static Stack      redoStack;
static char       namaFile[256];
static int        modified;

/* ============================================================
 * FUNGSI HELPER
 * ============================================================ */

/*
 * tanya_konfirmasi - Cetak pertanyaan y/n, return 1 jika user jawab y/Y
 */
static int tanya_konfirmasi(const char *pertanyaan) {
    char jawab[8];
    printf("%s (y/n): ", pertanyaan);
    fflush(stdout);
    if (!fgets(jawab, sizeof(jawab), stdin)) return 0;
    return (jawab[0] == 'y' || jawab[0] == 'Y');
}

/*
 * bersihkan_newline - Hapus \r dan \n di akhir string
 */
static void bersihkan_newline(char *str) {
    int len = (int)strlen(str);
    if (len > 0 && str[len-1] == '\n') str[--len] = '\0';
    if (len > 0 && str[len-1] == '\r') str[--len] = '\0';
}

/* ============================================================
 * HANDLER TIAP PERINTAH
 * ============================================================ */

/*
 * cmd_insert - Tambah teks di akhir baris aktif
 *
 * Opsi A: sscanf sudah memisahkan argumen dari perintah.
 * Satu spasi pertama adalah pemisah, sisanya adalah teks asli.
 *
 * Spasi antar kata ditangani otomatis:
 *   Jika baris aktif sudah ada isi (lineLength > 0),
 *   tambahkan ' ' sebelum teks baru.
 *   Jika baris masih kosong, langsung tambahkan teks.
 *
 * Contoh:
 *   Baris aktif: "Halo"
 *   > i dunia
 *   Hasil: "Halo dunia"   <- spasi otomatis
 *
 *   Baris aktif: ""
 *   > i Halo
 *   Hasil: "Halo"         <- tidak ada spasi ekstra
 */
static void cmd_insert(const char *teks) {
    int row;

    if (teks[0] == '\0') {
        printf("[INFO] Ketik teks setelah perintah i. Contoh: i Halo\n");
        return;
    }

    row = buf.currentRow;
    buffer_push_undo(&undoStack, &redoStack, &buf);

    /* Tambah spasi otomatis jika baris sudah ada isi */
    if (buf.lineLength[row] > 0) {
        buffer_insert(&buf, " ");
    }
    buffer_insert(&buf, teks);

    modified = 1;
    printf("[OK] Baris %d diperbarui.\n", row + 1);
    display_buffer(&buf, namaFile, modified);
}

/*
 * cmd_insert_baris - Buat baris baru di bawah baris aktif
 */
static void cmd_insert_baris(const char *teks) {
    buffer_push_undo(&undoStack, &redoStack, &buf);
    buffer_insert_baris(&buf, teks);
    modified = 1;
    printf("[OK] Baris baru dibuat di baris %d.\n", buf.currentRow + 1);
    display_buffer(&buf, namaFile, modified);
}

/*
 * cmd_hapus_karakter - Hapus n karakter terakhir di baris aktif
 *
 * Validasi input: jika argumen bukan angka valid atau negatif,
 * gunakan default 1. Tidak akan crash karena atoi("abc") = 0
 * dan kita clamp ke minimum 1.
 */
static void cmd_hapus_karakter(const char *argumen) {
    int jumlah = 1;

    if (argumen[0] != '\0') {
        jumlah = atoi(argumen);
        if (jumlah < 1) {
            printf("[INFO] Jumlah tidak valid, menggunakan default 1.\n");
            jumlah = 1;
        }
    }

    buffer_push_undo(&undoStack, &redoStack, &buf);
    buffer_backspace(&buf, jumlah);
    modified = 1;
    printf("[OK] %d karakter dihapus dari baris %d.\n",
           jumlah, buf.currentRow + 1);
    display_buffer(&buf, namaFile, modified);
}

/*
 * cmd_hapus_baris - Hapus seluruh baris aktif
 */
static void cmd_hapus_baris(void) {
    int baris_lama = buf.currentRow + 1;
    buffer_push_undo(&undoStack, &redoStack, &buf);
    buffer_hapus_baris(&buf);
    modified = 1;
    printf("[OK] Baris %d dihapus.\n", baris_lama);
    display_buffer(&buf, namaFile, modified);
}

/*
 * cmd_goto - Pindah ke baris nomor tertentu
 *
 * Validasi: jika argumen kosong atau bukan angka, beri pesan error.
 * buffer_goto() sudah melakukan clamp ke range yang valid.
 */
static void cmd_goto(const char *argumen) {
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

    buffer_goto(&buf, nomor);
    printf("[OK] Pindah ke baris %d.\n", buf.currentRow + 1);
    display_buffer(&buf, namaFile, modified);
}

/*
 * cmd_replace - Cari dan ganti teks dengan dua prompt terpisah
 *
 * Tidak menggunakan '|' sebagai pemisah.
 * Program bertanya satu per satu:
 *   Cari    : <user ketik>
 *   Ganti   : <user ketik>
 *
 * Prompt menggunakan ':' sebagai pemisah label-input,
 * konsisten dengan format tampilan.
 */
static void cmd_replace(void) {
    char cari[CMD_MAX];
    char ganti[CMD_MAX];
    int  jumlah;

    printf("  Cari    : ");
    fflush(stdout);
    if (!fgets(cari, sizeof(cari), stdin)) return;
    bersihkan_newline(cari);

    if (cari[0] == '\0') {
        printf("[BATAL] Teks yang dicari tidak boleh kosong.\n");
        return;
    }

    printf("  Ganti   : ");
    fflush(stdout);
    if (!fgets(ganti, sizeof(ganti), stdin)) return;
    bersihkan_newline(ganti);

    buffer_push_undo(&undoStack, &redoStack, &buf);
    jumlah = replace_text(&buf, cari, ganti);

    if (jumlah > 0) {
        modified = 1;
        printf("[OK] %d kemunculan '%s' diganti menjadi '%s'.\n",
               jumlah, cari, ganti);
        display_buffer(&buf, namaFile, modified);
    } else {
        printf("[INFO] Teks '%s' tidak ditemukan.\n", cari);
    }
}

/*
 * cmd_buka - Buka file dari disk
 */
static void cmd_buka(const char *argumen) {
    if (argumen[0] == '\0') {
        printf("[ERROR] Masukkan nama file. Contoh: o catatan.txt\n");
        return;
    }

    if (modified) {
        if (!tanya_konfirmasi("[PERINGATAN] Ada perubahan belum disimpan. Lanjut?"))
        {
            printf("[BATAL] Buka file dibatalkan.\n");
            return;
        }
    }

    if (file_open(&buf, argumen)) {
        strncpy(namaFile, argumen, sizeof(namaFile) - 1);
        namaFile[sizeof(namaFile) - 1] = '\0';
        stack_init(&undoStack);
        stack_init(&redoStack);
        modified = 0;
        printf("[OK] File '%s' dibuka (%d baris).\n", namaFile, buf.totalLines);
        display_buffer(&buf, namaFile, modified);
    } else {
        printf("[ERROR] File '%s' tidak ditemukan atau tidak bisa dibuka.\n",
               argumen);
    }
}

/*
 * cmd_simpan - Simpan buffer ke file di disk
 */
static void cmd_simpan(const char *argumen) {
    if (argumen[0] != '\0') {
        strncpy(namaFile, argumen, sizeof(namaFile) - 1);
        namaFile[sizeof(namaFile) - 1] = '\0';
    }

    if (namaFile[0] == '\0') {
        printf("[ERROR] Belum ada nama file. Gunakan: s <namafile>\n");
        return;
    }

    if (file_save(&buf, namaFile)) {
        modified = 0;
        printf("[OK] Disimpan ke '%s'.\n", namaFile);
    } else {
        printf("[ERROR] Gagal menyimpan '%s'.\n", namaFile);
    }
}

/*
 * cmd_tutup - Tutup file dan reset buffer
 */
static void cmd_tutup(void) {
    if (modified) {
        if (!tanya_konfirmasi("[PERINGATAN] Ada perubahan belum disimpan. Lanjut?"))
        {
            printf("[BATAL] Tutup file dibatalkan.\n");
            return;
        }
    }

    file_close(&buf);
    namaFile[0] = '\0';
    stack_init(&undoStack);
    stack_init(&redoStack);
    modified = 0;
    printf("[OK] Buffer dikosongkan.\n");
    display_buffer(&buf, namaFile, modified);
}

/*
 * cmd_hapus_file - Hapus file dari disk secara permanen
 *
 * Menggunakan remove() dari <stdio.h> — standar C99.
 * Meminta konfirmasi karena tidak bisa di-undo.
 */
static void cmd_hapus_file(const char *argumen) {
    if (argumen[0] == '\0') {
        printf("[ERROR] Masukkan nama file. Contoh: del catatan.txt\n");
        return;
    }

    printf("[PERINGATAN] '%s' akan dihapus PERMANEN dari disk.\n", argumen);
    if (!tanya_konfirmasi("Yakin?")) {
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

/*
 * cmd_undo - Batalkan perubahan terakhir
 */
static void cmd_undo(void) {
    if (buffer_undo(&undoStack, &redoStack, &buf)) {
        modified = 1;
        printf("[OK] Undo berhasil.\n");
        display_buffer(&buf, namaFile, modified);
    } else {
        printf("[INFO] Tidak ada yang bisa di-undo.\n");
    }
}

/*
 * cmd_redo - Ulangi perubahan yang di-undo
 */
static void cmd_redo(void) {
    if (buffer_redo(&undoStack, &redoStack, &buf)) {
        modified = 1;
        printf("[OK] Redo berhasil.\n");
        display_buffer(&buf, namaFile, modified);
    } else {
        printf("[INFO] Tidak ada yang bisa di-redo.\n");
    }
}

/*
 * cmd_keluar - Keluar dari program
 */
static void cmd_keluar(void) {
    if (modified) {
        if (!tanya_konfirmasi("[PERINGATAN] Ada perubahan belum disimpan. Keluar?"))
        {
            printf("[BATAL] Keluar dibatalkan.\n");
            return;
        }
    }
    printf("Sampai jumpa!\n");
    exit(0);
}

/* ============================================================
 * DISPATCHER
 * Terima satu baris input, pecah jadi perintah + argumen,
 * panggil handler yang sesuai.
 * ============================================================ */
static void proses_perintah(char *input) {
    char perintah[16];
    char argumen[CMD_MAX];

    argumen[0]  = '\0';
    perintah[0] = '\0';

    sscanf(input, "%15s %511[^\n]", perintah, argumen);
    if (perintah[0] == '\0') return;

    if      (strcmp(perintah, "i")   == 0) cmd_insert(argumen);
    else if (strcmp(perintah, "ia")  == 0) cmd_insert_baris(argumen);
    else if (strcmp(perintah, "d")   == 0) cmd_hapus_karakter(argumen);
    else if (strcmp(perintah, "dl")  == 0) cmd_hapus_baris();
    else if (strcmp(perintah, "g")   == 0) cmd_goto(argumen);
    else if (strcmp(perintah, "f")   == 0) cmd_replace();
    else if (strcmp(perintah, "o")   == 0) cmd_buka(argumen);
    else if (strcmp(perintah, "s")   == 0) cmd_simpan(argumen);
    else if (strcmp(perintah, "w")   == 0) cmd_tutup();
    else if (strcmp(perintah, "del") == 0) cmd_hapus_file(argumen);
    else if (strcmp(perintah, "u")   == 0) cmd_undo();
    else if (strcmp(perintah, "r")   == 0) cmd_redo();
    else if (strcmp(perintah, "h")   == 0) display_bantuan();
    else if (strcmp(perintah, "q")   == 0) cmd_keluar();
    else {
        printf("[ERROR] Perintah '%s' tidak dikenal. Ketik h untuk bantuan.\n",
               perintah);
    }
}

/* ============================================================
 * MAIN
 * ============================================================ */
int main(int argc, char *argv[]) {
    char cmd[CMD_MAX];
    int  len;

    /* ----- Inisialisasi ----- */
    buffer_init(&buf);
    stack_init(&undoStack);
    stack_init(&redoStack);
    namaFile[0] = '\0';
    modified    = 0;

    /* ----- Layar pembuka ----- */
    printf("\n=== Text Editor CLI  |  Array 2D + Stack ===\n\n");

    /* ----- Tampilkan bantuan di awal ----- */
    display_bantuan();

    /* ----- Buka file dari argumen jika ada ----- */
    if (argc > 1) {
        if (file_open(&buf, argv[1])) {
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

    /* Tampilkan buffer awal */
    display_buffer(&buf, namaFile, modified);

    /* ----- Main loop ----- */
    while (1) {
        /*
         * Prompt format: [namafile* | brs X/Y]>
         *   *  = ada perubahan belum disimpan
         *   X  = baris aktif saat ini
         *   Y  = total baris
         */
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

        proses_perintah(cmd);
    }

    return 0;
}
