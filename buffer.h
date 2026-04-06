#ifndef BUFFER_H
#define BUFFER_H

/*
 * buffer.h — Array 2D teks + operasi insert/delete + stack undo/redo
 *
 * Representasi fisik : Alokasi Sequential (Array Statis 2D)
 *
 * Struktur data utama:
 *   text[MAX_ROW][MAX_COL] → array 2D, setiap baris adalah text[row]
 *   lineLength[MAX_ROW]    → panjang tiap baris
 *   totalLines             → jumlah baris aktif
 *
 * Undo/Redo menggunakan dua Stack berbasis array statis:
 *   undoStack[HISTORY_SIZE] → PUSH sebelum perubahan, POP saat undo
 *   redoStack[HISTORY_SIZE] → PUSH saat undo, POP saat redo
 *   Kondisi kosong : top = 0
 *   Kondisi penuh  : top = HISTORY_SIZE (OVERFLOW)
 */

#define MAX_ROW      500
#define MAX_COL      200
#define HISTORY_SIZE  20

/* ===== Struktur Data Utama ===== */

typedef struct {
    char text[MAX_ROW][MAX_COL];
    int  lineLength[MAX_ROW];
    int  totalLines;
    int  currentRow;   /* baris aktif (posisi "cursor") */
} TextBuffer;

typedef struct {
    TextBuffer buf;
    int        currentRow;
} Snapshot;

typedef struct {
    Snapshot entries[HISTORY_SIZE];
    int      top;  /* 0 = kosong, HISTORY_SIZE = penuh (overflow) */
} Stack;

/* ===== API Buffer ===== */

void buffer_init(TextBuffer *buf);

/* Insert teks di akhir baris aktif */
void buffer_insert(TextBuffer *buf, const char *teks);

/* Insert baris baru di bawah baris aktif */
void buffer_insert_baris(TextBuffer *buf, const char *teks);

/* Hapus N karakter terakhir di baris aktif (backspace) */
void buffer_backspace(TextBuffer *buf, int n);

/* Hapus seluruh baris aktif */
void buffer_hapus_baris(TextBuffer *buf);

/* Pindah ke baris tertentu (1-based) */
void buffer_goto(TextBuffer *buf, int nomor);

/* ===== API Stack (Undo/Redo) ===== */

void stack_init(Stack *s);

/* PUSH snapshot ke stack */
void stack_push(Stack *s, const TextBuffer *buf);

/* POP snapshot dari stack. Return 1 jika berhasil, 0 jika kosong. */
int  stack_pop(Stack *s, TextBuffer *buf);

/* ===== API Undo/Redo ===== */

void buffer_push_undo(Stack *undo, Stack *redo_reset, const TextBuffer *buf);
int  buffer_undo(Stack *undo, Stack *redo, TextBuffer *buf);
int  buffer_redo(Stack *undo, Stack *redo, TextBuffer *buf);

#endif /* BUFFER_H */
