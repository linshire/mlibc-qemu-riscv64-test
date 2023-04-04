#ifndef __STRING_H_
#define __STRING_H_

#include <typedef.h>

size_t strlen(const char *s);
void *memset (void *dstpp, int c, size_t len);
void *memcpy(void *dst, const void *src, size_t count);
void *memccpy(void *dst, const void *src, unsigned char cha, size_t count);

#endif
