#ifndef __PIPE_H
#define __PIPE_H

#define _GNU_SOURCE

#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>

void pipe_init(int size);
void pipe_write(char c);
int pipe_read(char* c);
void pipe_destroy();
void pipe_writeDone();

#endif