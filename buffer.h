#ifndef BUFFER_H
#define BUFFER_H

#include <stdlib.h>
#define MAX_COL      200
#define HISTORY_SIZE  20


typedef struct Node {
    char text[MAX_COL];
    int length;
    struct Node *next;
} Node;

typedef struct {
    Node *head;
    int totalLines;
    int currentRow;
} TextBuffer;

typedef struct {
    Node *head;
    int totalLines;
    int currentRow;
} Snapshot;

typedef struct {
    Snapshot entries[HISTORY_SIZE];
    int top;
} Stack;

Node *allocNode(void);
void  freeNoden(Node *node);

void  bufferInsertBaris(TextBuffer *buf, char *teks);
void  bufferHapusBaris(TextBuffer *buf);

Node *getNode(TextBuffer *buf, int n);
void  bufferGoton(TextBuffer *buf, int nomor);
void  bufferInsert(TextBuffer *buf, char *teks);
void  bufferBackspacen(TextBuffer *buf, int n);

void  bufferInit(TextBuffer *buf);
void  stackInit(Stack *s);
void  stackPushn(Stack *s, TextBuffer *buf);
int   stackPop (Stack *s, TextBuffer *buf);
void  bufferPushUndo(Stack *undo, Stack *redo, TextBuffer *buf);
int   bufferUndom(Stack *undo, Stack *redo, TextBuffer *buf);
int   bufferRedo (Stack *undo, Stack *redo, TextBuffer *buf);

#endif