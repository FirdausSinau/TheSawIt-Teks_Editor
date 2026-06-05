#ifndef FILE_H
#define FILE_H

#include "buffer.h"

int  fileOpen(TextBuffer *buf, char *filename);

int  fileSave(TextBuffer *buf, char *filename);

void fileClose(TextBuffer *buf);

#endif