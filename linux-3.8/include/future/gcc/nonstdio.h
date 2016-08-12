/*
 * Copyright (C) Paul Mackerras 1997.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version
 * 2 of the License, or (at your option) any later version.
 *
 * This is sort of a catchall for I/O related functions.  Stuff that
 * wouldn't be in 'stdio.h' normally is here, and it's 'nonstdio.h'
 * for a reason.  -- Tom
 */
#ifndef __NONSTDIO_H__
#define __NONSTDIO_H__

#include <stdarg.h>
#include <future/general.h>

typedef int FILE;
extern FILE *stdin, *stdout, *stderr;

#define NULL ((void *)0)
#define EOF (-1)
#define fopen(n, m) NULL
//#define fflush(f)	do {} while (0)
//#define fclose(f)	do {} while (0)
//#define ftell(f) 0
#define fseek(f, o, w) 0

int fprintf(FILE *fp, const char *fmt, ...);

#define printf(fmt,a...) debug(0, fmt, ##a)

extern int getc(void);
extern int sprintf(char *str, const char *format, ...);
extern int tstc(void);
//extern void exit(int);
//extern void outb(int port, unsigned char val);
extern int putc(int c, FILE *fp);
extern void puthex(unsigned long val);
extern int puts(const char *);
//extern void udelay(long delay);
//extern unsigned char inb(int port);
extern void board_isa_init(void);
extern void ISA_init(unsigned long base);

size_t fwrite(void *buf, size_t len, size_t i, FILE *fp);
int fputs(const char *buf, FILE *fp);
int fputc(int buf, FILE *fp);
unsigned long ftell(FILE *fp);
#if 0 
inline void fflush(FILE *fp);
inline int fclose(FILE *fp);
#else
void fflush(FILE *fp);
int fclose(FILE *fp);
#endif

int mkdir( const char *dirname );
#define mkdir(a,b)      mkdir(a)
#endif /* __NONSTDIO_H__ */
