#include "buffer.h"
#include <string.h>
#include <stdio.h>

void buffer_init(TextBuffer *buf) {
    memset(buf, 0, sizeof(TextBuffer));
    buf->totalLines = 1;
}

void buffer_insert(TextBuffer *buf, const char *teks) {
    int row = buf->currentRow;
    int len = strlen(teks);

    for (int i = 0; i < len && buf->lineLength[row] < MAX_COL - 1; i++) {
        buf->text[row][buf->lineLength[row]++] = teks[i];
    }

    buf->text[row][buf->lineLength[row]] = '\0';
}

void buffer_insert_baris(TextBuffer *buf, const char *teks) {
    int row = buf->currentRow;

    if (buf->totalLines >= MAX_ROW) {
        printf("[OVERFLOW] Buffer penuh.\n");
        return;
    }

    for (int i = buf->totalLines; i > row + 1; i--) {
        memcpy(buf->text[i], buf->text[i - 1], MAX_COL);
        buf->lineLength[i] = buf->lineLength[i - 1];
    }

    buf->totalLines++;
    buf->currentRow = row + 1;

    int len = strlen(teks);
    if (len >= MAX_COL) len = MAX_COL - 1;

    memcpy(buf->text[buf->currentRow], teks, len);
    buf->text[buf->currentRow][len] = '\0';
    buf->lineLength[buf->currentRow] = len;
}

void buffer_backspace(TextBuffer *buf, int n) {
    int row = buf->currentRow;

    while (n-- > 0 && buf->lineLength[row] > 0) {
        buf->text[row][--buf->lineLength[row]] = '\0';
    }
}

void buffer_hapus_baris(TextBuffer *buf) {
    int row = buf->currentRow;

    if (buf->totalLines == 1) {
        buf->lineLength[0] = 0;
        buf->text[0][0] = '\0';
        return;
    }

    for (int i = row; i < buf->totalLines - 1; i++) {
        memcpy(buf->text[i], buf->text[i + 1], MAX_COL);
        buf->lineLength[i] = buf->lineLength[i + 1];
    }

    memset(buf->text[buf->totalLines - 1], 0, MAX_COL);
    buf->lineLength[buf->totalLines - 1] = 0;
    buf->totalLines--;

    if (buf->currentRow >= buf->totalLines) {
        buf->currentRow = buf->totalLines - 1;
    }
}

void buffer_goto(TextBuffer *buf, int nomor) {
    if (nomor < 1) nomor = 1;
    if (nomor > buf->totalLines) nomor = buf->totalLines;
    buf->currentRow = nomor - 1;
}

void buffer_display(const TextBuffer *buf) {
    for (int i = 0; i < buf->totalLines; i++) {
        printf("%3d | %s\n", i + 1, buf->text[i]);
    }
}

void stack_init(Stack *s) {
    s->top = 0;
}

void stack_push(Stack *s, const TextBuffer *buf) {
    if (s->top >= HISTORY_SIZE) {
        memmove(&s->entries[0], &s->entries[1], (HISTORY_SIZE - 1) * sizeof(Snapshot));
        s->top = HISTORY_SIZE - 1;
    }

    memcpy(&s->entries[s->top].buf, buf, sizeof(TextBuffer));
    s->top++;
}

int stack_pop(Stack *s, TextBuffer *buf) {
    if (s->top == 0) return 0;

    memcpy(buf, &s->entries[--s->top].buf, sizeof(TextBuffer));
    return 1;
}

void buffer_push_undo(Stack *undo, Stack *redo, const TextBuffer *buf) {
    stack_push(undo, buf);
    redo->top = 0;
}

int buffer_undo(Stack *undo, Stack *redo, TextBuffer *buf) {
    if (undo->top == 0) return 0;

    stack_push(redo, buf);
    return stack_pop(undo, buf);
}

int buffer_redo(Stack *undo, Stack *redo, TextBuffer *buf) {
    if (redo->top == 0) return 0;

    stack_push(undo, buf);
    return stack_pop(redo, buf);
}