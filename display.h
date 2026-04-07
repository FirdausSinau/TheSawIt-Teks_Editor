#ifndef DISPLAY_H
#define DISPLAY_H

#include "buffer.h"

void display_buffer(const TextBuffer *buf,
                    const char       *namaFile,
                    int               modified);

void display_bantuan(void);

#endif