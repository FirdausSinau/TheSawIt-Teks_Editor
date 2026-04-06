/*
 * buffer.c — Implementasi array 2D teks + insert/delete + stack undo/redo
 *
 * Representasi fisik : Alokasi Sequential (Array Statis 2D)
 *
 * Array 2D text[row][col]:
 *   - Setiap baris teks = satu baris array text[row]
 *   - Urutan baris diwakili oleh indeks fisik (posisi sequential)
 *   - Insert karakter  → geser elemen ke kanan dari posisi col
 *   - Hapus baris      → geser semua baris di bawah ke atas
 *   - Insert baris     → geser semua baris di bawah ke bawah
 *
 * Stack undo/redo:
 *   - Representasi fisik : Array statis Snapshot[HISTORY_SIZE]
 *   - Jenis struktur     : Stack (insert & delete hanya di TOP)
 *   - top = 0           → stack kosong
 *   - top = HISTORY_SIZE → stack penuh (OVERFLOW)
 */

#include "buffer.h"
#include <string.h>
#include <stdio.h>

/* ===== Inisialisasi ===== */

void buffer_init(TextBuffer *buf) {
    memset(buf->text, 0, sizeof(buf->text));
    memset(buf->lineLength, 0, sizeof(buf->lineLength));
    buf->totalLines  = 1;
    buf->currentRow  = 0;
    buf->lineLength[0] = 0;
}

/* ===== Operasi Insert ===== */

/*
 * buffer_insert — Tambahkan teks di akhir baris aktif
 *
 * Algoritma:
 *   hitung panjang teks yang akan ditambahkan
 *   salin karakter satu per satu ke text[currentRow] mulai dari lineLength
 *   update lineLength[currentRow]
 */
void buffer_insert(TextBuffer *buf, const char *teks) {
    int row = buf->currentRow;
    int len = (int)strlen(teks);
    int i;

    for (i = 0; i < len; i++) {
        if (buf->lineLength[row] >= MAX_COL - 1) break; /* baris penuh */
        buf->text[row][buf->lineLength[row]] = teks[i];
        buf->lineLength[row]++;
    }
    buf->text[row][buf->lineLength[row]] = '\0';
}

/*
 * buffer_insert_baris — Sisipkan baris baru di bawah baris aktif
 *
 * Algoritma INSERT ke array sequential:
 *   1. Cek apakah totalLines sudah penuh (OVERFLOW)
 *   2. Geser semua baris di bawah currentRow ke bawah satu posisi
 *      (dari baris paling bawah ke atas, supaya tidak tumpang tindih)
 *   3. Isi baris baru di currentRow + 1
 *   4. totalLines++, currentRow++
 */
void buffer_insert_baris(TextBuffer *buf, const char *teks) {
    int row = buf->currentRow;
    int len, i;

    if (buf->totalLines >= MAX_ROW) {
        printf("[OVERFLOW] Buffer penuh, tidak bisa menambah baris.\n");
        return;
    }

    /* Geser baris di bawah ke bawah (dari bawah ke atas) */
    for (i = buf->totalLines; i > row + 1; i--) {
        memcpy(buf->text[i], buf->text[i - 1], MAX_COL);
        buf->lineLength[i] = buf->lineLength[i - 1];
    }
    buf->totalLines++;

    /* Isi baris baru */
    buf->currentRow = row + 1;
    len = (int)strlen(teks);
    if (len > MAX_COL - 1) len = MAX_COL - 1;
    memcpy(buf->text[buf->currentRow], teks, len);
    buf->text[buf->currentRow][len] = '\0';
    buf->lineLength[buf->currentRow] = len;
}

/* ===== Operasi Delete ===== */

/*
 * buffer_backspace — Hapus N karakter terakhir di baris aktif
 *
 * Algoritma:
 *   ulangi N kali:
 *     jika lineLength > 0:
 *       kurangi lineLength
 *       set karakter terakhir ke '\0'
 */
void buffer_backspace(TextBuffer *buf, int n) {
    int row = buf->currentRow;
    int i;

    for (i = 0; i < n; i++) {
        if (buf->lineLength[row] == 0) break; /* UNDERFLOW: baris sudah kosong */
        buf->lineLength[row]--;
        buf->text[row][buf->lineLength[row]] = '\0';
    }
}

/*
 * buffer_hapus_baris — Hapus baris aktif dari array sequential
 *
 * Algoritma DELETE dari array sequential:
 *   1. Cek UNDERFLOW: jika hanya 1 baris, kosongkan saja isinya
 *   2. Geser semua baris di bawah currentRow ke atas satu posisi
 *      (dari atas ke bawah, supaya tidak tumpang tindih)
 *   3. Bersihkan baris terakhir yang sudah tidak dipakai
 *   4. totalLines--
 *   5. Sesuaikan currentRow jika perlu
 */
void buffer_hapus_baris(TextBuffer *buf) {
    int row = buf->currentRow;
    int i;

    if (buf->totalLines == 1) {
        /* UNDERFLOW: hanya 1 baris, kosongkan saja */
        buf->lineLength[0] = 0;
        buf->text[0][0]    = '\0';
        return;
    }

    /* Geser semua baris di bawah ke atas */
    for (i = row; i < buf->totalLines - 1; i++) {
        memcpy(buf->text[i], buf->text[i + 1], MAX_COL);
        buf->lineLength[i] = buf->lineLength[i + 1];
    }

    /* Bersihkan baris terakhir yang sudah tidak terpakai */
    memset(buf->text[buf->totalLines - 1], 0, MAX_COL);
    buf->lineLength[buf->totalLines - 1] = 0;
    buf->totalLines--;

    /* Sesuaikan currentRow jika hapus baris terakhir */
    if (buf->currentRow >= buf->totalLines)
        buf->currentRow = buf->totalLines - 1;
}

/* ===== Navigasi ===== */

/*
 * buffer_goto — Pindah ke baris nomor tertentu (1-based)
 */
void buffer_goto(TextBuffer *buf, int nomor) {
    if (nomor < 1) nomor = 1;
    if (nomor > buf->totalLines) nomor = buf->totalLines;
    buf->currentRow = nomor - 1;
}

/* ===== Stack (untuk Undo/Redo) ===== */

void stack_init(Stack *s) {
    s->top = 0; /* top = 0 → stack kosong */
}

/*
 * stack_push — PUSH snapshot ke stack
 *
 * Algoritma PUSH:
 *   if (top = HISTORY_SIZE) → OVERFLOW:
 *     geser semua entri ke kiri (buang entri terlama di indeks 0)
 *     top ← top - 1
 *   stack[top] ← data
 *   top ← top + 1
 */
void stack_push(Stack *s, const TextBuffer *buf) {
    int i;

    /* Cek OVERFLOW */
    if (s->top >= HISTORY_SIZE) {
        /* Geser semua entri ke kiri, buang yang terlama */
        for (i = 0; i < HISTORY_SIZE - 1; i++) {
            s->entries[i] = s->entries[i + 1];
        }
        s->top = HISTORY_SIZE - 1;
    }

    /* Salin snapshot ke posisi top */
    memcpy(&s->entries[s->top].buf, buf, sizeof(TextBuffer));
    s->entries[s->top].currentRow = buf->currentRow;
    s->top++;
}

/*
 * stack_pop — POP snapshot dari stack
 *
 * Algoritma POP:
 *   if (top = 0) → UNDERFLOW: return 0
 *   top ← top - 1
 *   data ← stack[top]
 *   return 1
 */
int stack_pop(Stack *s, TextBuffer *buf) {
    if (s->top == 0) return 0; /* UNDERFLOW: stack kosong */

    s->top--;
    memcpy(buf, &s->entries[s->top].buf, sizeof(TextBuffer));
    buf->currentRow = s->entries[s->top].currentRow;
    return 1;
}

/* ===== Undo / Redo ===== */

/*
 * buffer_push_undo — Simpan state saat ini ke undoStack
 * Reset redoStack karena operasi baru membatalkan semua redo.
 */
void buffer_push_undo(Stack *undo, Stack *redo_reset, const TextBuffer *buf) {
    stack_push(undo, buf);
    redo_reset->top = 0; /* reset redo stack */
}

/*
 * buffer_undo — Kembalikan ke state sebelumnya
 *
 * Algoritma:
 *   PUSH state saat ini ke redoStack
 *   POP dari undoStack → restore ke buf
 */
int buffer_undo(Stack *undo, Stack *redo, TextBuffer *buf) {
    if (undo->top == 0) return 0; /* tidak ada yang bisa di-undo */

    stack_push(redo, buf);   /* simpan state saat ini ke redo */
    stack_pop(undo, buf);    /* restore dari undo */
    return 1;
}

/*
 * buffer_redo — Ulang operasi yang di-undo
 *
 * Algoritma:
 *   PUSH state saat ini ke undoStack
 *   POP dari redoStack → restore ke buf
 */
int buffer_redo(Stack *undo, Stack *redo, TextBuffer *buf) {
    if (redo->top == 0) return 0; /* tidak ada yang bisa di-redo */

    stack_push(undo, buf);   /* simpan state saat ini ke undo */
    stack_pop(redo, buf);    /* restore dari redo */
    return 1;
}
