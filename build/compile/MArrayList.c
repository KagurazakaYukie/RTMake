#include "../include/compile/MArrayList.h"
#include <stdlib.h>
#include <stdbool.h>

MArrayList *MArrayListInit(MTMemoryManager *mm) {
    MemoryInfo *ma = MTMemoryManagerCalloc(mm, sizeof(MArrayList));
    MArrayList *mal = (MArrayList *) ma->m;
    mal->Element = NULL;
    mal->mal = ma;
    mal->mi = NULL;
    mal->length = 0;
    return mal;
}

MArrayList *MBUArrayListInit(MemoryBigUnit *mbu) {
    MemoryInfo *ma = MTMemoryManagerUnitCalloc(mbu, sizeof(MArrayList));
    MArrayList *mal = (MArrayList *) ma->m;
    mal->Element = NULL;
    mal->mal = ma;
    mal->mi = NULL;
    mal->length = 0;
    return mal;
}

MArrayList *GetMArrayList(MemoryInfo *mi) {
    return (MArrayList *) mi->m;
}

void *MArrayListGetIndex(MArrayList *arraylist, size_t i) {
    if (i <= (arraylist->length - 1)) {
        return arraylist->Element[i];
    } else {
        return NULL;
    }
}

bool MArrayListSetIndex(MArrayList *arraylist, size_t i, void *element) {
    if (i <= (arraylist->length - 1)) {
        arraylist->Element[i] = element;
        return true;
    } else {
        return false;
    }
}

size_t MArrayListSize(MArrayList *arraylist) {
    return arraylist->length;
}

void MArrayListDestroy(MTMemoryManager *mm, MArrayList *arraylist) {
    if (arraylist->mi != NULL) {
        MTMemoryManagerFree(mm, arraylist->mi);
    }
    MTMemoryManagerFree(mm, arraylist->mal);
}

void MBUArrayListDestroy(MemoryBigUnit *mbu, MArrayList *arraylist) {
    if (arraylist->mi != NULL) {
        MTMemoryManagerUnitFree(mbu, arraylist->mi);
    }
    MTMemoryManagerUnitFree(mbu, arraylist->mal);
}

void MArrayListAllDestroy(MTMemoryManager *mm, MArrayList *arraylist) {
    if (arraylist->mi != NULL) {
        for (int i = 0; i < arraylist->length; i++) {
            MTMemoryManagerFree(mm, (MemoryInfo *) MArrayListGetIndex(arraylist, i));
        }
        MTMemoryManagerFree(mm, arraylist->mi);
    }
    MTMemoryManagerFree(mm, arraylist->mal);
}

void MBUArrayListAllDestroy(MemoryBigUnit *mbu, MArrayList *arraylist) {
    if (arraylist->mi != NULL) {
        for (int i = 0; i < arraylist->length; i++) {
            MTMemoryManagerUnitFree(mbu, (MemoryInfo *) MArrayListGetIndex(arraylist, i));
        }
        MTMemoryManagerUnitFree(mbu, arraylist->mi);
    }
    MTMemoryManagerUnitFree(mbu, arraylist->mal);
}


bool MArrayListDeleteIndex(MTMemoryManager *mm, MArrayList *arraylist, void *element,ListCmp cmp) {
    int i = -1;
    for (int ii = 0; ii < arraylist->length; ii++) {
        if (cmp(((MemoryInfo *) arraylist->Element[ii])->m, ((MemoryInfo *) element)->m)==0) {
            i = ii;
            break;
        }
    }
    if (i >= 0) {
        MTMemoryManagerFree(mm, (MemoryInfo *) arraylist->Element[i]);
        for (; i < arraylist->length; i++) {
            arraylist->Element[i] = arraylist->Element[i + 1];
        }
        arraylist->length--;
    }
    return true;
}

bool MBUArrayListDeleteIndex(MemoryBigUnit *mbu, MArrayList *arraylist, void *element,ListCmp cmp) {
    int i = -1;
    for (int ii = 0; ii < arraylist->length; ii++) {
        if (cmp(((MemoryInfo *) arraylist->Element[ii])->m, ((MemoryInfo *) element)->m)==0) {
            i = ii;
            break;
        }
    }
    if (i >= 0) {
        MTMemoryManagerUnitFree(mbu, (MemoryInfo *) arraylist->Element[i]);
        for (; i < arraylist->length; i++) {
            arraylist->Element[i] = arraylist->Element[i + 1];
        }
        arraylist->length--;
    }
    return true;
}

bool MArrayListDeleteAppointIndex(MTMemoryManager *mm, MArrayList *arraylist, size_t i) {
    if (i >= 0) {
        if (arraylist->Element[i] != NULL) {
            MTMemoryManagerFree(mm, (MemoryInfo *) arraylist->Element[i]);
        }
        for (; i < arraylist->length; i++) {
            arraylist->Element[i] = arraylist->Element[i + 1];
        }
        arraylist->length--;
        return true;
    } else {
        return false;
    }
}

bool MBUArrayListDeleteAppointIndex(MemoryBigUnit *mbu, MArrayList *arraylist, size_t i) {
    if (i >= 0) {
        if (arraylist->Element[i] != NULL) {
            MTMemoryManagerUnitFree(mbu, (MemoryInfo *) arraylist->Element[i]);
        }
        for (; i < arraylist->length; i++) {
            arraylist->Element[i] = arraylist->Element[i + 1];
        }
        arraylist->length--;
        return true;
    } else {
        return false;
    }
}

bool MArrayListDeleteIndexNoFree(MArrayList *arraylist, void *element,ListCmp cmp) {
    int i = -1;
    for (int ii = 0; ii < arraylist->length; ii++) {
        if (cmp(((MemoryInfo *) arraylist->Element[ii])->m, ((MemoryInfo *) element)->m)==0) {
            i = ii;
            break;
        }
    }
    if (i >= 0) {
        for (; i < arraylist->length; i++) {
            arraylist->Element[i] = arraylist->Element[i + 1];
        }
        arraylist->length--;
    }
    return true;
}

bool MArrayListDeleteAppointIndexNoFree(MArrayList *arraylist, size_t i) {
    if (i >= 0) {
        for (; i < arraylist->length; i++) {
            arraylist->Element[i] = arraylist->Element[i + 1];
        }
        arraylist->length--;
        return true;
    } else {
        return false;
    }
}

bool MArrayListAddIndexs(MTMemoryManager *mm, MArrayList *arraylist, void **element, size_t size) {
    int p = 0;
    if (arraylist->mi == NULL) {
        if (ListLength > size) {
            MemoryInfo *mi = MTMemoryManagerCalloc(mm, ListLength << sizeof(void*));
            arraylist->Element = mi->m;
            for (size_t i = arraylist->length; i < arraylist->length + size; i++) {
                arraylist->Element[i] = element[p];
                p++;
            }
            arraylist->length += size;
            return true;
        }
        MemoryInfo *mi = MTMemoryManagerCalloc(mm,
                                               (((size >> ListBinaryLength) << 1) << ListBinaryLength << sizeof(void*)));
        arraylist->Element = mi->m;
        for (size_t i = arraylist->length; i < arraylist->length + size; i++) {
            arraylist->Element[i] = element[p];
            p++;
        }
        arraylist->length += size;
        return true;
    }
    if ((arraylist->length + size) > (arraylist->mi->size >> sizeof(void*))) {
        arraylist->mi = MTMemoryManagerRealloc(mm, arraylist->mi, arraylist->mi->size +
                                                                  (((size >> ListBinaryLength) << 1) << ListBinaryLength
                                                                                                     << sizeof(void*)));
        arraylist->Element = arraylist->mi->m;
        for (size_t i = arraylist->length; i < arraylist->length + size; i++) {
            arraylist->Element[i] = element[p];
            p++;
        }
        arraylist->length += size;
        return true;
    }
    for (size_t i = arraylist->length; i < arraylist->length + size; i++) {
        arraylist->Element[i] = element[p];
        p++;
    }
    arraylist->length += size;
    return true;
}

bool MBUArrayListAddIndexs(MemoryBigUnit *mbu, MArrayList *arraylist, void **element, size_t size) {
    int p = 0;
    if (arraylist->mi == NULL) {
        if (ListLength > size) {
            MemoryInfo *mi = MTMemoryManagerUnitCalloc(mbu, ListLength << sizeof(void*));
            arraylist->Element = mi->m;
            for (size_t i = arraylist->length; i < arraylist->length + size; i++) {
                arraylist->Element[i] = element[p];
                p++;
            }
            arraylist->length += size;
            return true;
        }
        MemoryInfo *mi =MTMemoryManagerUnitCalloc(mbu,(((size >> ListBinaryLength) << 1) << ListBinaryLength << sizeof(void*)));
        arraylist->Element = mi->m;
        for (size_t i = arraylist->length; i < arraylist->length + size; i++) {
            arraylist->Element[i] = element[p];
            p++;
        }
        arraylist->length += size;
        return true;
    }
    if ((arraylist->length + size) > (arraylist->mi->size >> sizeof(void*))) {
        arraylist->mi = MTMemoryManagerUnitRealloc(mbu, arraylist->mi, arraylist->mi->size +(((size >> ListBinaryLength) << 1) << ListBinaryLength<< sizeof(void*)));
        arraylist->Element = arraylist->mi->m;
        for (size_t i = arraylist->length; i < arraylist->length + size; i++) {
            arraylist->Element[i] = element[p];
            p++;
        }
        arraylist->length += size;
        return true;
    }
    for (size_t i = arraylist->length; i < arraylist->length + size; i++) {
        arraylist->Element[i] = element[p];
        p++;
    }
    arraylist->length += size;
    return true;
}

bool MArrayListAddIndex(MTMemoryManager *mm, MArrayList *arraylist, void *element) {
    if (arraylist->mi == NULL) {
        arraylist->mi = MTMemoryManagerCalloc(mm, (ListLength << sizeof(void*)));
        arraylist->Element = arraylist->mi->m;
        arraylist->Element[0] = element;
        arraylist->length++;
        return true;
    }
    if ((arraylist->length + 1) > (arraylist->mi->size >> sizeof(void*))) {
        arraylist->mi = MTMemoryManagerRealloc(mm, arraylist->mi, arraylist->mi->size + (ListLength << sizeof(void*)));
        arraylist->Element = arraylist->mi->m;
    }
    arraylist->Element[arraylist->length] = element;
    arraylist->length++;
    return true;
}

bool MBUArrayListAddIndex(MemoryBigUnit *mbu, MArrayList *arraylist, void *element) {
    if (arraylist->mi == NULL) {
        arraylist->mi = MTMemoryManagerUnitCalloc(mbu, (ListLength << sizeof(void*)));
        arraylist->Element = arraylist->mi->m;
        arraylist->Element[0] = element;
        arraylist->length++;
        return true;
    }
    if ((arraylist->length + 1) > (arraylist->mi->size >> sizeof(void*))) {
        arraylist->mi = MTMemoryManagerUnitRealloc(mbu, arraylist->mi, arraylist->mi->size + (ListLength << sizeof(void*)));
        arraylist->Element = arraylist->mi->m;
    }
    arraylist->Element[arraylist->length] = element;
    arraylist->length++;
    return true;
}

bool MArrayListInsertIndex(MTMemoryManager *mm, MArrayList *arraylist, size_t position, void *element) {
    if (position <= arraylist->length) {
        if ((arraylist->length + 1) > (arraylist->mi->size >> sizeof(void*))) {
            arraylist->mi = MTMemoryManagerRealloc(mm, arraylist->mi,
                                                   arraylist->mi->size + (ListLength << sizeof(void*)));
        }
        for (size_t i = (arraylist->mi->size >> sizeof(void*)) - 1; i > position; i--) {
            arraylist->Element[i] = arraylist->Element[i - 1];
        }
        arraylist->Element[position] = element;
        arraylist->length++;
        return true;
    } else {
        return false;
    }
}

bool MBUArrayListInsertIndex(MemoryBigUnit *mbu, MArrayList *arraylist, size_t position, void *element) {
    if (position <= arraylist->length) {
        if ((arraylist->length + 1) > (arraylist->mi->size >> sizeof(void*))) {
            arraylist->mi = MTMemoryManagerUnitRealloc(mbu, arraylist->mi,
                                                   arraylist->mi->size + (ListLength << sizeof(void*)));
        }
        for (size_t i = (arraylist->mi->size >> sizeof(void*)) - 1; i > position; i--) {
            arraylist->Element[i] = arraylist->Element[i - 1];
        }
        arraylist->Element[position] = element;
        arraylist->length++;
        return true;
    } else {
        return false;
    }
}