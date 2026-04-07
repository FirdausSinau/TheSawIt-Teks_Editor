#ifndef BUFFER_H
#define BUFFER_H
#define MAX_ROW      500
#define MAX_COL      200
#define HISTORY_SIZE  20



typedef struct {
    char text[MAX_ROW][MAX_COL];
    int  lineLength[MAX_ROW];
    int  totalLines;
    int  currentRow;  
} TextBuffer;

typedef struct {
    TextBuffer buf;
    
} Snapshot;

typedef struct {
    Snapshot entries[HISTORY_SIZE];
    int      top; 
} Stack;


void buffer_init(TextBuffer *buf);

void buffer_insert(TextBuffer *buf, const char *teks);

void buffer_insert_baris(TextBuffer *buf, const char *teks);

void buffer_backspace(TextBuffer *buf, int n);

void buffer_hapus_baris(TextBuffer *buf);

void buffer_goto(TextBuffer *buf, int nomor);

void stack_init(Stack *s);

void stack_push(Stack *s, const TextBuffer *buf);

int  stack_pop(Stack *s, TextBuffer *buf);


void buffer_push_undo(Stack *undo, Stack *redo_reset, const TextBuffer *buf);
int  buffer_undo(Stack *undo, Stack *redo, TextBuffer *buf);
int  buffer_redo(Stack *undo, Stack *redo, TextBuffer *buf);

<<<<<<< HEAD
#endif 
=======
#endif 
>>>>>>> cfc70fc60063e08479e1129a245c30cecbb4166b
