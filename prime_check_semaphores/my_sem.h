#ifndef __MYSEM_H
#define __MYSEM_H


#include <stdio.h>
#include <sys/sem.h>
#include <sys/stat.h>

int mysem_create(int );
void mysem_down(int );
int mysem_up(int );
void mysem_destroy(int );
int mysem_value(int );

#endif
