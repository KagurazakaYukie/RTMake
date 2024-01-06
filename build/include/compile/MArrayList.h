#ifndef MARRAYLISET_H
#define MARRAYLISET_H

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <bits/wordsize.h>
#include "MArrayList.h"
#include "MultitThreadMemoryManager.h"

#define ListBinaryLength 4
#define ListLength 32

typedef void *(*ListCmp)(void *, void *);

typedef struct MAL {
    MemoryInfo *mi, *mal;
    void **Element;
    size_t length;
} MArrayList;

MArrayList *MArrayListInit(MTMemoryManager *mm);

MArrayList *MBUArrayListInit(MemoryBigUnit *mbu);

MArrayList *GetMArrayList(MemoryInfo *mi);

void *MArrayListGetIndex(MArrayList *arraylist, size_t i);

bool MArrayListSetIndex(MArrayList *arraylist, size_t i, void *element);

size_t MArrayListSize(MArrayList *arraylist);

void MArrayListDestroy(MTMemoryManager *mm, MArrayList *arraylist);

void MBUArrayListDestroy(MemoryBigUnit *mbu, MArrayList *arraylist);

void MArrayListAllDestroy(MTMemoryManager *mm, MArrayList *arraylist);

void MBUArrayListAllDestroy(MemoryBigUnit *mbu, MArrayList *arraylist);

bool MArrayListDeleteIndex(MTMemoryManager *mm, MArrayList *arraylist, void *element,ListCmp cmp);

bool MBUArrayListDeleteIndex(MemoryBigUnit *mbu, MArrayList *arraylist, void *element,ListCmp cmp);

bool MArrayListDeleteAppointIndex(MTMemoryManager *mm, MArrayList *arraylist, size_t i);

bool MBUArrayListDeleteAppointIndex(MemoryBigUnit *mbu, MArrayList *arraylist, size_t i);

bool MArrayListDeleteIndexNoFree(MArrayList *arraylist, void *element,ListCmp cmp);

bool MArrayListDeleteAppointIndexNoFree(MArrayList *arraylist, size_t i);

bool MArrayListAddIndexs(MTMemoryManager *mm, MArrayList *arraylist, void **element, size_t size);

bool MBUArrayListAddIndexs(MemoryBigUnit *mbu, MArrayList *arraylist, void **element, size_t size);

bool MArrayListAddIndex(MTMemoryManager *mm, MArrayList *arraylist, void *element);

bool MBUArrayListAddIndex(MemoryBigUnit *mbu, MArrayList *arraylist, void *element);

bool MArrayListInsertIndex(MTMemoryManager *mm, MArrayList *arraylist, size_t position, void *element);

bool MBUArrayListInsertIndex(MemoryBigUnit *mbu, MArrayList *arraylist, size_t position, void *element);

#endif