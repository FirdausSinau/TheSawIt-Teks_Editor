
#include "buffer.h"
#include <string.h>
#include <stdio.h>


void buffer_init(TextBuffer *buf) {
    memset(buf->text, 0, sizeof(buf->text));
    memset(buf->lineLength, 0, sizeof(buf->lineLength));
    buf->totalLines  = 1;
    buf->currentRow  = 0;
    buf->lineLength[0] = 0;
}

void buffer_insert(TextBuffer *buf, const char *teks) {
    int row = buf->currentRow;
    int len = (int)strlen(teks);
    int i;

    for (i = 0; i < len; i++) {
        if (buf->lineLength[row] >= MAX_COL - 1) break; 
        buf->text[row][buf->lineLength[row]] = teks[i];
        buf->lineLength[row]++;
    }
    buf->text[row][buf->lineLength[row]] = '\0';
}

void buffer_insert_baris(TextBuffer *buf, const char *teks) {
    int row = buf->currentRow;
    int len, i;

    if (buf->totalLines >= MAX_ROW) {
        printf("[OVERFLOW] Buffer penuh, tidak bisa menambah baris.\n");
        return;
    }

   
    for (i = buf->totalLines; i > row + 1; i--) {
        memcpy(buf->text[i], buf->text[i - 1], MAX_COL);
        buf->lineLength[i] = buf->lineLength[i - 1];
    }
    buf->totalLines++;


    buf->currentRow = row + 1;
    len = (int)strlen(teks);
    if (len > MAX_COL - 1) len = MAX_COL - 1;
    memcpy(buf->text[buf->currentRow], teks, len);
    buf->text[buf->currentRow][len] = '\0';
    buf->lineLength[buf->currentRow] = len;
}


void buffer_backspace(TextBuffer *buf, int n) {
    int row = buf->currentRow;
    int i;

    for (i = 0; i < n; i++) {
        if (buf->lineLength[row] == 0) break; 
        buf->lineLength[row]--;
        buf->text[row][buf->lineLength[row]] = '\0';
    }
}


void buffer_hapus_baris(TextBuffer *buf) {
    int row = buf->currentRow;
    int i;

    if (buf->totalLines == 1) {
       
        buf->lineLength[0] = 0;
        buf->text[0][0]    = '\0';
        return;
    }


    for (i = row; i < buf->totalLines - 1; i++) {
        memcpy(buf->text[i], buf->text[i + 1], MAX_COL);
        buf->lineLength[i] = buf->lineLength[i + 1];
    }

    
    memset(buf->text[buf->totalLines - 1], 0, MAX_COL);
    buf->lineLength[buf->totalLines - 1] = 0;
    buf->totalLines--;

    if (buf->currentRow >= buf->totalLines)
        buf->currentRow = buf->totalLines - 1;
}


void buffer_goto(TextBuffer *buf, int nomor) {
    if (nomor < 1) nomor = 1;
    if (nomor > buf->totalLines) nomor = buf->totalLines;
    buf->currentRow = nomor - 1;
}

void stack_init(Stack *s) {
    s->top = 0; 
}

void stack_push(Stack *s, const TextBuffer *buf) {
    int i;

    
    if (s->top >= HISTORY_SIZE) {
        
        for (i = 0; i < HISTORY_SIZE - 1; i++) {
            s->entries[i] = s->entries[i + 1];
        }
        s->top = HISTORY_SIZE - 1;
    }

   
    memcpy(&s->entries[s->top].buf, buf, sizeof(TextBuffer));
    s->entries[s->top].currentRow = buf->currentRow;
    s->top++;
}


int stack_pop(Stack *s, TextBuffer *buf) {
    if (s->top == 0) return 0; 

    s->top--;
    memcpy(buf, &s->entries[s->top].buf, sizeof(TextBuffer));
    buf->currentRow = s->entries[s->top].currentRow;
    return 1;
}


void buffer_push_undo(Stack *undo, Stack *redo_reset, const TextBuffer *buf) {
    stack_push(undo, buf);
    redo_reset->top = 0; 
}


int buffer_undo(Stack *undo, Stack *redo, TextBuffer *buf) {
    if (undo->top == 0) return 0; 

    stack_push(redo, buf);  
    stack_pop(undo, buf);   
    return 1;
}


int buffer_redo(Stack *undo, Stack *redo, TextBuffer *buf) {
    if (redo->top == 0) return 0; 
    
    stack_push(undo, buf);   
    stack_pop(redo, buf);    
    return 1;
}
