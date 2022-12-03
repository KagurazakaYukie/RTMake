#ifndef RTMAKE_COMPILE_H
#define RTMAKE_COMPILE_H

#include <stdio.h>
#include <stdlib.h>
#include "MArrayList.h"
#include "MultitThreadMemoryManager.h"

#define Clang 0
#define Gcc 1

#define O1 1
#define O2 2
#define O3 3

#define Linux 0

#define Run 0
#define So 1
#define A 2

typedef struct af {
    MemoryInfo *m,*ff1,*ff2;
    MArrayList *p1,*p2;
} AutoFree;

void Mkdirss(char *mkdirm, char *file, int os);

void CpFile(char *cpm, char *file, char *tofile, int os);

MemoryInfo *CCompile(MTMemoryManager *mm, int gccorclang, char *file, int type, char *diy, int o, int os);

int RTMakeAllO(MTMemoryManager *mm, char *file, MArrayList *listc,int o);

MArrayList *Pretreatment(MTMemoryManager *mm,char *file,char *sfile,MArrayList *clist,MArrayList *flist);

MArrayList *AutoFreeConfig(MTMemoryManager *mm, char *file);

void MemoryStrcat(MTMemoryManager *mm, MemoryInfo **m, char *str);

void installlib(MTMemoryManager *mm,char *path,char *name);

MArrayList *ClistGet(MTMemoryManager *mm, char *file, MemoryInfo *sfile, MArrayList *cl);

#endif