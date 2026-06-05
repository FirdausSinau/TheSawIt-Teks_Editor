# Text Editor CLI v2 — Linked List

## Deskripsi

Text Editor CLI ini adalah aplikasi text editor berbasis terminal (CLI) yang dibuat menggunakan bahasa C. Proyek ini dikembangkan untuk menerapkan konsep struktur data **Singly Linked List**, di mana setiap baris teks disimpan dalam satu node yang terhubung.

Berbeda dengan editor sebelumnya yang menyimpan teks dalam array dua dimensi, aplikasi ini menggunakan linked list sehingga proses penambahan, penghapusan, dan pengelolaan baris dapat dilakukan secara lebih fleksibel tanpa perlu menggeser seluruh elemen.

---

## Fitur

Beberapa fitur yang tersedia pada aplikasi ini antara lain:

- Menambahkan teks di akhir baris aktif.
- Membuat baris baru di bawah baris aktif.
- Menghapus karakter terakhir (dengan jumlah yang dapat ditentukan).
- Menghapus seluruh baris aktif.
- Navigasi ke baris nomor tertentu.
- Mencari dan mengganti teks pada seluruh dokumen (Find and Replace).
- Membuka file dari disk.
- Menyimpan dokumen ke file.
- Menutup file dan mengosongkan buffer.
- Menghapus file dari disk secara permanen.
- Fitur Undo dan Redo hingga 20 langkah terakhir.
- Menampilkan daftar perintah yang tersedia.

---

## Struktur Data yang Digunakan

Aplikasi dibangun menggunakan struktur data **Singly Linked List** yang terdiri dari beberapa komponen utama.

### Node

Digunakan untuk menyimpan satu baris teks lengkap beserta panjangnya dan pointer ke baris berikutnya.

```c
typedef struct Node {
    char text[MAX_COL];     /* Menyimpan maksimal 200 karakter per baris */
    int length;             /* Panjang teks yang efektif */
    struct Node *next;      /* Pointer ke baris selanjutnya */
} Node;
```

### TextBuffer

Struktur utama yang mengelola keseluruhan dokumen, mencatat baris pertama, jumlah total baris, dan posisi baris aktif.

```c
typedef struct {
    Node *head;         /* Pointer ke baris pertama */
    int totalLines;     /* Jumlah total baris */
    int currentRow;     /* Indeks baris yang sedang aktif (0-based) */
} TextBuffer;
```

### Snapshot

Digunakan untuk menyimpan salinan dari kondisi `TextBuffer` pada suatu waktu tertentu sebagai bagian dari mekanisme Undo/Redo.

```c
typedef struct {
    Node *head;         /* Salinan linked list baris */
    int totalLines;     /* Salinan jumlah baris */
    int currentRow;     /* Salinan posisi baris aktif */
} Snapshot;
```

### Stack

Struktur untuk menyimpan hingga 20 snapshot sebagai riwayat perubahan (Undo Stack dan Redo Stack).

```c
typedef struct {
    Snapshot entries[HISTORY_SIZE];   /* Array snapshot */
    int top;                          /* Posisi snapshot teratas */
} Stack;
```

---

## Daftar Perintah

| Perintah | Keterangan | Contoh |
|---|---|---|
| `i <teks>` | Menambahkan teks di akhir baris aktif | `i Halo dunia` |
| `ia <teks>` | Membuat baris baru di bawah baris aktif | `ia Baris baru` |
| `d [n]` | Menghapus `n` karakter terakhir. Default: 1 | `d 5` |
| `dl` | Menghapus seluruh baris aktif | `dl` |
| `g <nomor>` | Berpindah ke baris dengan nomor tertentu | `g 3` |
| `f` | Mencari dan mengganti teks (2 prompt) | `f` |
| `o <file>` | Membuka file dari disk | `o catatan.txt` |
| `s [file]` | Menyimpan file ke disk | `s catatan.txt` |
| `w` | Menutup file / mengosongkan buffer | `w` |
| `del <file>` | Menghapus file dari disk (permanen) | `del lama.txt` |
| `u` | Undo — membatalkan perubahan terakhir | `u` |
| `r` | Redo — mengulangi perubahan yang dibatalkan | `r` |
| `h` | Menampilkan daftar perintah | `h` |
| `q` | Keluar dari program | `q` |

---

## Batasan Program

- Maksimal **200 karakter per baris**. Teks yang melebihi batas ini akan dipotong secara otomatis.
- Riwayat Undo/Redo menyimpan maksimal **20 langkah terakhir**. Langkah yang lebih lama akan dihapus secara otomatis.
- Navigasi kursor pada satu baris belum mendukung pergerakan ke kiri dan kanan secara per karakter.

---

## Struktur File Project

| File | Keterangan |
|---|---|
| `main.c` | Fungsi utama, parser perintah, dan loop interaksi dengan pengguna |
| `Anand.c` | Alokasi node, penambahan baris baru, dan penghapusan baris |
| `Firdaus.c` | Navigasi node, penambahan dan penghapusan karakter per baris |
| `Faiq.c` | Inisialisasi buffer, implementasi stack, snapshot, undo, dan redo |
| `file.c` | Operasi file: membuka, menyimpan, dan menutup file |
| `replace.c` | Implementasi fitur pencarian dan penggantian teks |
| `display.c` | Menampilkan isi buffer, header informasi, dan daftar bantuan |
| `buffer.h` | Definisi struktur data (`Node`, `TextBuffer`, `Stack`, dll) dan deklarasi fungsi |
| `file.h` | Deklarasi fungsi operasi file |
| `replace.h` | Deklarasi fungsi pencarian dan penggantian teks |
| `display.h` | Deklarasi fungsi tampilan |
