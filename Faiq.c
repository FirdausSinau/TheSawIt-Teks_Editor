#include "buffer.h"

void bufferInit(TextBuffer *buf) {
	Node *awal = (Node *)malloc(sizeof(Node)); 

    if (awal == NULL){ 
        printf("ERROR! Gagal Alokasi. Program tidak bisa dilanjutkan.");
        buf->head = NULL; 
        buf->totalLines = 0; 
        buf->currentRow = 0; 
        return; 
    }

    awal->text[0] = '\0'; 
    awal->length = 0; 
    awal->next = NULL; 

    buf->head = awal; 
    buf->totalLines = 1; 
    buf->currentRow = 0; 
}

void stackInit(Stack *s){
    int i; 
    Node *cur; 
    Node *tmp; 

    for (i = 0; i < s->top; i++){ 
        cur = s->entries[i].head; 
        while(cur != NULL){ 
            tmp = cur->next; 
            free(cur); 
            cur = tmp; 
        }
        s->entries[i].head = NULL; 
    }

	s->top = 0; 
}

void stackPush(Stack *s, TextBuffer *buf) { 
    int   j; 
    Node *cur; 
    Node *newNode; 
    Node *snapHead = NULL; 
    Node *snapTail = NULL;

    if (s->top >= HISTORY_SIZE) { 
        Node *del = s->entries[0].head; 
        while (del != NULL) { 
            Node *tmp = del->next; 
            free(del); 
            del = tmp; 
        }
  
        for (j = 0; j < HISTORY_SIZE - 1; j++) { 
            s->entries[j] = s->entries[j + 1]; 
        }
        s->top = HISTORY_SIZE - 1; 
    }

    cur = buf->head;

    while (cur != NULL) {
        newNode = (Node *)malloc(sizeof(Node)); 
        for (j = 0; j <= cur->length; j++) {
            newNode->text[j] = cur->text[j]; 
        }
        newNode->length = cur->length; 
        newNode->next   = NULL; 
        
        if (snapHead == NULL) { 
            snapHead = newNode; 
            snapTail = newNode; 
        } else { 
            snapTail->next = newNode; 
            snapTail       = newNode; 
        }

        cur = cur->next; 
    }

    s->entries[s->top].head       = snapHead; 
    s->entries[s->top].totalLines = buf->totalLines; 
    s->entries[s->top].currentRow = buf->currentRow;
    s->top++; 
}

int stackPop(Stack *s, TextBuffer *buf){ 
	Node *del; 
    Node *temp; 

    if(s->top == 0){ 
        return 0;
    }

    
    del = buf->head; 

    while(del != NULL){ 
        temp = del->next; 
        free(del); 
        del = temp; 
    }

    s->top--;
    buf->head = s->entries[s->top].head; 
    buf->totalLines = s->entries[s->top].totalLines; 
    buf->currentRow = s->entries[s->top].currentRow; 

    s->entries[s->top].head = NULL; 
    s->entries[s->top].totalLines = 0; 
    s->entries[s->top].currentRow = 0; 

    return 1; 
}

void  bufferPushUndo(Stack *undo, Stack *redo, TextBuffer *buf){
    int   i;
    Node *del;
    Node *tmp;

    stackPush(undo, buf);

    while (redo->top > 0) {
        redo->top--;
        del = redo->entries[redo->top].head;
        while (del != NULL) {
            tmp = del->next;
            free(del);
            del = tmp;
        }
        redo->entries[redo->top].head = NULL;
    }
}

int bufferUndo(Stack *undo, Stack *redo, TextBuffer *buf){
	if (undo->top == 0) return 0;
    stackPush(redo, buf);
    return stackPop(undo, buf);
}

int bufferRedo(Stack *undo, Stack *redo, TextBuffer *buf){
	if (redo->top == 0) return 0;
    stackPush(undo, buf);
    return stackPop(redo, buf);
}

