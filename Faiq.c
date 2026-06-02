#include "buffer.h"

void bufferInit(TextBuffer *buf) {
	buf->head = NULL;
	buf->totalLines = 0;
	buf->currentRow = 0;
}

void stackInit(Stack *s){
	s->top = 0;
}

void stackPush(Stack *s, TextBuffer *buf){
	int j; //loop Counter
	Node* cur; //Pointer Traversal
	Node* newNode; //Pointer Alokasi Node Baru
	Node* snapHead = NULL; //Pointer ke head stack
	Node* snapTail = NULL; //Pointer ke tail stack
	
	if(s->top >= HISTORY_SIZE){
		Node* del = s->entries[0].head;
		
		while(del != NULL){
			Node *tmp = del->next;
			free(del);
			del = temp;
		}
		
		for(j = 0; j < HISTORY_SIZE - 1; j++){
			s->entries[j] = s->entries[j + 1]; 
		}
		s->top = HISTORY_SIZE - 1;
		
		cur = buf->head;
		
		while(cur != NULL){
			
		}
		
	}
}

int stackPop(Stack *s, TextBuffer *buf){
	
}

int bufferUndo(Stack *undo, Stack *redo, TextBuffer *buf){
	
}

int bufferRedo(Stack *undo, Stack *redo, TextBuffer *buf){
	
}

