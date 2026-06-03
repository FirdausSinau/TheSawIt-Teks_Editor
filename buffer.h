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
void  freeNode(Node *node);

void  bufferInsertBaris(TextBuffer *buf, char *teks);
void  bufferHapusBaris(TextBuffer *buf);

Node *getNode(TextBuffer *buf, int n);
void  bufferGoto(TextBuffer *buf, int nomor);
void  bufferInsert(TextBuffer *buf, char *teks);
void  bufferBackspace(TextBuffer *buf, int n);

void  bufferInit(TextBuffer *buf);
void  stackInit(Stack *s);
void  stackPush(Stack *s, TextBuffer *buf);
int   stackPop(Stack *s, TextBuffer *buf);
void  bufferPushUndo(Stack *undo, Stack *redo, TextBuffer *buf);
int   bufferUndo(Stack *undo, Stack *redo, TextBuffer *buf);
int   bufferRedo(Stack *undo, Stack *redo, TextBuffer *buf);

#endif