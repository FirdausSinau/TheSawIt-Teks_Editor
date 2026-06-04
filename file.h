#ifndef FILE_H
#define FILE_H

#include "buffer.h"

int  fileOpen(TextBuffer *buf, const char *filename);

int  fileSave(const TextBuffer *buf, const char *filename);

void fileClose(TextBuffer *buf);

#endif