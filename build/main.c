#include <stdio.h>
#include <unistd.h>
#include "include/compile/Command.h"
#include "include/compile/MultitThreadMemoryManager.h"
#include <pthread.h>

int main(int argc, char *argv[]) {
    MTMemoryManager *mm = MTMemoryManagerInit(1024 * 1024);
    MemoryBigUnit *mbu = MTMemoryManagerBindingThread(mm, pthread_self());

    MemoryInfo *path = MTMemoryManagerCalloc(mm, 2048);
    getcwd(path->m, 2048);

    char *ctmp = NULL, *tmp;
    char *name = strtok_r(path->m, "/", &ctmp);
    while (name != NULL) {
        if ((tmp = strtok_r(NULL, "/", &ctmp)) == NULL) {
            break;
        } else {
            name = tmp;
        }

    }

    getcwd(path->m, 2048);
    if (argc == 1) {
        MArrayList *flist = AutoFreeConfig(mm, path->m);
        printf("AutoFreeConfig end\n");
        MArrayList *clist = Pretreatment(mm, path->m, NULL, NULL, flist);
        printf("Pretreatment end\n");
        for (int i = 0; i < MArrayListSize(clist); ++i) {
            printf("C List:%s\n", (char *) ((MemoryInfo *) MArrayListGetIndex(clist, i))->m);
        }
        if (!RTMakeAllO(mm, path->m, clist, O3)) {
        printf("RTMakeAllO end\n");
                CCompile(mm, Clang, path->m, Run, NULL, O3, Linux);
                printf("Run end\n");
                CCompile(mm, Clang, path->m, So, NULL, O3, Linux);
                printf("So end\n");
                CCompile(mm, Clang, path->m, A, NULL, O3, Linux);
                printf("A end\n");
            }
    }
    if (argc == 2) {
        if (strcmp(argv[1], "-H") == 0) {
            printf("help\n");
        }

        if (strcmp(argv[1], "-I") == 0) {
            MArrayList *flist = AutoFreeConfig(mm, path->m);

            MArrayList *clist = Pretreatment(mm, path->m, NULL, NULL, flist);

            for (int i = 0; i < MArrayListSize(clist); ++i) {
                printf("C List:%s \n", (char *) ((MemoryInfo *) MArrayListGetIndex(clist, i))->m);
            }

            if (!RTMakeAllO(mm, path->m, clist, O3)) {
                CCompile(mm, Clang, path->m, Run, NULL, O3, Linux);
                CCompile(mm, Clang, path->m, So, NULL, O3, Linux);
                CCompile(mm, Clang, path->m, A, NULL, O3, Linux);
                installlib(mm, path->m, name);
            }
        }

        if (strcmp(argv[1], "-P") == 0) {
            MArrayList *flist = AutoFreeConfig(mm, path->m);
            MArrayList *clist = Pretreatment(mm, path->m, NULL, NULL, flist);
            for (int i = 0; i < MArrayListSize(clist); ++i) {
                printf("C List:%s \n", (char *) ((MemoryInfo *) MArrayListGetIndex(clist, i))->m);
            }
        }

        if (strcmp(argv[1], "-O") == 0) {
            MArrayList *clist = ClistGet(mm, path->m, NULL, NULL);
            for (int i = 0; i < MArrayListSize(clist); ++i) {
                printf("C List:%s \n", (char *) ((MemoryInfo *) MArrayListGetIndex(clist, i))->m);
            }
            RTMakeAllO(mm, path->m, clist, O3);
        }

        if (strcmp(argv[1], "-R") == 0) {
            CCompile(mm, Clang, path->m, Run, NULL, O3, Linux);
        }

        if (strcmp(argv[1], "-L") == 0) {
            CCompile(mm, Clang, path->m, So, NULL, O3, Linux);
        }

        if (strcmp(argv[1], "-A") == 0) {
            CCompile(mm, Clang, path->m, A, NULL, O3, Linux);
        }

    }
}
