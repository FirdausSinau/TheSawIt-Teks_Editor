#include "file.h"
#include <stdio.h>
#include <string.h>


int file_open(TextBuffer *buf, const char *filename) {
    FILE *fp;
    char  barisTemp[MAX_COL + 4];
    int   row = 0;
    int   len;

    fp = fopen(filename, "r");
    if (!fp) return 0;

    buffer_init(buf);


    while (fgets(barisTemp, (int)sizeof(barisTemp), fp) && row < MAX_ROW) {
        len = (int)strlen(barisTemp);
        if (len > 0 && barisTemp[len - 1] == '\n') barisTemp[--len] = '\0';
        if (len > 0 && barisTemp[len - 1] == '\r') barisTemp[--len] = '\0';
        if (len > MAX_COL - 1) len = MAX_COL - 1;
        memcpy(buf->text[row], barisTemp, len);
        buf->text[row][len] = '\0';
        buf->lineLength[row] = len;
        row++;
    }
    buf->totalLines  = (row == 0) ? 1 : row;
    buf->currentRow  = 0;
    fclose(fp);
    return 1;
}


int file_save(const TextBuffer *buf, const char *filename) {
    FILE *fp;
    int   row;

    fp = fopen(filename, "w");
    if (!fp) return 0;

    for (row = 0; row < buf->totalLines; row++) {
        fputs(buf->text[row], fp);
        if (row < buf->totalLines - 1) fputc('\n', fp);
    }

    fclose(fp);
    return 1;
}


void file_close(TextBuffer *buf) {
    buffer_init(buf);
}