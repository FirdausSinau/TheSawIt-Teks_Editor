#ifndef FILE_H
#define FILE_H

#include "buffer.h"

int  file_open(TextBuffer *buf, const char *filename);

int  file_save(const TextBuffer *buf, const char *filename);

void file_close(TextBuffer *buf);

#endif