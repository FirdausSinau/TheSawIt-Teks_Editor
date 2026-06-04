#include "buffer.h"

void bufferInit(TextBuffer *buf) {
	Node *awal = (Node *)malloc(sizeof(Node)); //malloc sebuah node bernama awal di memori 

    awal->text[0] = '\0'; //isi indeks 0 dengan null terminator agar bersih dari sampah memori 
    awal->length = 0; //nilai panjang baris set ke 0
    awal->next = NULL; //next dari node awal set ke NULL

    buf->head = awal; //head dari buf arahkan ke node awal
    buf->totalLines = 1; //nilai total baris buf set ke 1
    buf->currentRow = 0; //nilai currentrow set di 0
}

void stackInit(Stack *s){
    int i; //variabel traversing indeks array stack snapshot
    Node *cur; //penunjuk traversal di stack snapshot
    Node *tmp; //pemegang list next dari cur

    for (i = 0; i < s->top; i++){ //loop tiap slot snapshot yang terisi sampe top-1
        cur = s->entries[i].head; //cur nunjuk ke head linked list salinan (snapshot) ke-i
        while(cur != NULL){ //selama cur belum nunjuk NULL
            tmp = cur->next; //tmp tampung list next dari cur
            free(cur); //free node yang ditunjuk cur
            cur = tmp; //cur maju ke next node
        }
        s->entries[i].head = NULL; //setelah node kosong (free) arahin pointer head ke NULL
    }

	s->top = 0; //nilai top dari stack snapshot di set ke 0 
}

void stackPush(Stack *s, TextBuffer *buf) { //
    int   j; //variabel indeks loop
    Node *cur; //pointer traversal (yang sedang ditunjuk) untuk linked list buffer asi
    Node *newNode; //pointer penunjuk node baru (salinan) yg akan dialokasikan
    Node *snapHead = NULL; //head dari linked list salinan buffer
    Node *snapTail = NULL; //tail dari linked list salinan buffer

    if (s->top >= HISTORY_SIZE) { //cek apakah stack snapshot sudah penuh (20)
        Node *del = s->entries[0].head; //pointer del nunjuk ke head snapshot
        while (del != NULL) { //selama pointer del tidak nunjuk ke NULL
            Node *tmp = del->next; //pointer tmp memegang list snapshot mulai dari next-nya del agar tidak hilang semua
            free(del); //menghapus node del saat ini 
            del = tmp; //del pindah ke node berikutnya yang sedang dipegang tmp
        }
  
        for (j = 0; j < HISTORY_SIZE - 1; j++) { //dari j=0 sampai j=19
            s->entries[j] = s->entries[j + 1]; //geser snapshot ke indeks yang lebih lama (awal)
        }
        s->top = HISTORY_SIZE - 1; //top nunjuk ke indeks array stack yang kosong
    }

    cur = buf->head; //pointer cur nujuk ke head (baris pertama) dari linked list buffer

    while (cur != NULL) { //selama cur ga nunjuk ke NULL
        newNode = (Node *)malloc(sizeof(Node)); //alokasi slot di memori untuk newNode (node salinan dari cur)
        for (j = 0; j <= cur->length; j++) { //untuk j=0 sampai j<=panjang (jumlah) string pada array text
            newNode->text[j] = cur->text[j]; //salin text dari node-node di list text buffer ke snapshot (salinan)
        }
        newNode->length = cur->length; //nilai panjang untuk node snapshot diisi dengan nilai panjang node asli buffer 
        newNode->next   = NULL; //next dari node snapshot set ke NULL

        if (snapHead == NULL) { //jika pointer snapHead masih di NULL
            snapHead = newNode; //head dari list snapshot megang node salinan list buffer
            snapTail = newNode; //tail juga sama, karena baru satu node di list snapshot
        } else { //kalau head snapshot udah nunjuk ke node
            snapTail->next = newNode; //sambungin node baru 
            snapTail       = newNode; //snaptail arahin ke node baru
        }

        cur = cur->next; //cur arahin ke node selanjutnya di buffer untuk disalin
    }

    s->entries[s->top].head       = snapHead; //simpan head dari list snapshot ke array stack 
    s->entries[s->top].totalLines = buf->totalLines; //simpan nilai jumlah baris dari buffer saat proses snapshot terjadi
    s->entries[s->top].currentRow = buf->currentRow; //simpan indikator baris aktif saat snapshot terjadi
    s->top++; //top naik satu indeks
}

int stackPop(Stack *s, TextBuffer *buf){ //hapus sebuah list snapshot untuk dikirim kembali ke list buffer
	Node *del; //pointer traversal penunjuk node buffer yang akan dibebaskan
    Node *temp; //pointer pemegang list next dari del

    if(s->top == 0){ //cek isi stack snapshot, jika kosong
        return 0; //return 0, tidak ada snapshot yang bisa di pop
    }

    s->top--;
    del = buf->head; //pointer traversal del diarahkan ke head dari list buffer

    while(del != NULL){ //selama del belum nunjuk ke NULL
        temp = del->next; //temp pegang list dari next-nya del
        free(del); //free node yang ditunjuk del
        del = temp; //del nunjuk yang ditunjuk temp
    }

    buf->head = s->entries[s->top].head; //head buffer nunjuk yang ditunjuk head snapshot di stack bagian top
    buf->totalLines = s->entries[s->top].totalLines; //nilai totalLines buffer diisi nilai totalLines snapshot di stack bagian top
    buf->currentRow = s->entries[s->top].currentRow; //nilai currentRow buffer diisi nilai currentRow snapshot di stack bagian top

    s->entries[s->top].head = NULL; //head dari list snapshot top dari stack arahkan ke NULL
    s->entries[s->top].totalLines = 0; //nilai totalLines nya reset ke 0
    s->entries[s->top].currentRow = 0; //nilai currentRow nya reset ke 0

    return 1; //return 1, stackPop berhasil dijalankan
}

int bufferUndo(Stack *undo, Stack *redo, TextBuffer *buf){
	
}

int bufferRedo(Stack *undo, Stack *redo, TextBuffer *buf){
	
}

