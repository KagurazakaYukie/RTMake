#include "../include/compile/Command.h"
#include <dirent.h>
#include <string.h>

void Mkdirss(char *mkdirm, char *file, int os) {
    if (os == 0) {
        strcat(mkdirm, "if [ ! -d ");
        strcat(mkdirm, file);
        strcat(mkdirm, " ]; then\n");
        strcat(mkdirm, "mkdir ");
        strcat(mkdirm, file);
        strcat(mkdirm, "\nfi");
    }
}

void CpFile(char *cpm, char *file, char *tofile, int os) {
    if (os == 0) {
        strcat(cpm, "cp ");
        strcat(cpm, file);
        strcat(cpm, " ");
        strcat(cpm, tofile);
    }
}

void MemoryStrcat(MTMemoryManager *mm, MemoryInfo **m, char *str) {
    if ((*m)->size >= strlen((*m)->m) + strlen(str) + 2) {
        //printf("%p len:%d %d %d\n",(*m),strlen((*m)->m),strlen(str),strlen((*m)->m)+strlen(str) + 2);
        strcat((*m)->m, str);
    } else {
        *m = MTMemoryManagerRealloc(mm, *m, ((*m)->size * 2) + strlen(str) + 1);
        //printf("relen:%d\n",((*m)->size * 2) + strlen(str) + 1);
        strcat((*m)->m, str);
    }
}

void MemoryStrcpy(MTMemoryManager *mm, MemoryInfo **m, char *str) {
    if ((*m)->size >= strlen((*m)->m) + strlen(str) + 2) {
        //printf("%p len:%d %d %d\n",(*m),strlen((*m)->m),strlen(str),strlen((*m)->m)+strlen(str) + 2);
        strcpy((*m)->m, str);
    } else {
        *m = MTMemoryManagerRealloc(mm, *m, ((*m)->size * 2) + strlen(str) + 1);
        //printf("relen:%d\n",((*m)->size * 2) + strlen(str) + 1);
        strcpy((*m)->m, str);
    }
}

MArrayList *AutoFreeConfig(MTMemoryManager *mm, char *file) {
    MArrayList *freelist = MArrayListInit(mm);
    MemoryInfo *path = MTMemoryManagerCalloc(mm, 1024);
    MemoryStrcpy(mm, &path, file);
    MemoryStrcat(mm, &path, "/AutoFree");
    FILE *f = fopen(path->m, "r");
    if (!f) {
        return NULL;
    }
    char tmp[1024];
    char *a = NULL, *b = NULL, *a1 = NULL, *a2 = NULL, *b1 = NULL, *b2 = NULL, *t = NULL, *ctmp = NULL;
    MemoryInfo *mi = NULL;
    AutoFree *free = NULL;

    while (fgets(tmp, 1024, f) != NULL) {
        a = strtok_r(tmp, "@", &ctmp);
        b = strtok_r(NULL, "@", &ctmp);
        a[strlen(a) - 1] = '\0';
        b[strlen(b) - 2] = '\0';

        a1 = strtok_r(a, "(", &ctmp);
        a2 = strtok_r(NULL, "(", &ctmp);

        b1 = strtok_r(b, "(", &ctmp);
        b2 = strtok_r(NULL, "(", &ctmp);


        mi = MTMemoryManagerCalloc(mm, sizeof(AutoFree));
        free = (AutoFree *) mi->m;
        free->m = mi;
        free->ff1 = MTMemoryManagerCalloc(mm, strlen(a1) + 1);
        free->ff2 = MTMemoryManagerCalloc(mm, strlen(b1) + 1);
        MemoryStrcpy(mm, &free->ff1, a1);
        MemoryStrcpy(mm, &free->ff2, b1);
        free->p1 = MArrayListInit(mm);
        free->p2 = MArrayListInit(mm);

        t = strtok_r(a2, ",", &ctmp);
        while (t != NULL) {
            MemoryInfo *p = MTMemoryManagerCalloc(mm, strlen(t) + 1);
            MemoryStrcpy(mm, &p, t);
            MArrayListAddIndex(mm, free->p1, p);
            t = strtok_r(NULL, ",", &ctmp);
        }

        t = strtok_r(b2, ",", &ctmp);
        while (t != NULL) {
            MemoryInfo *p = MTMemoryManagerCalloc(mm, strlen(t) + 1);
            MemoryStrcpy(mm, &p, t);
            MArrayListAddIndex(mm, free->p2, p);
            t = strtok_r(NULL, ",", &ctmp);
        }

        MArrayListAddIndex(mm, freelist, free);
    }
    fclose(f);

    MTMemoryManagerFree(mm, path);
    return freelist;
}

MArrayList *Pretreatment(MTMemoryManager *mm, char *file, char *sfile, MArrayList *cl, MArrayList *flist) {
    MArrayList *clist = cl;
    if (clist == NULL) {
        clist = MArrayListInit(mm);
    }
    MemoryInfo *f = MTMemoryManagerCalloc(mm, 1024);
    MemoryInfo *build = MTMemoryManagerCalloc(mm, 1024);
    MemoryInfo *bin = MTMemoryManagerCalloc(mm, 1024);
    MemoryInfo *src = MTMemoryManagerCalloc(mm, 1024);
    MemoryInfo *inm = MTMemoryManagerCalloc(mm, 1024);
    MemoryInfo *tmp = MTMemoryManagerCalloc(mm, 1024);
    MemoryInfo *autofreemem = MTMemoryManagerCalloc(mm, 1024);

    MArrayList *plist = MArrayListInit(mm);

    MemoryInfo *sfilem = NULL;

    MemoryStrcpy(mm, &f, file);
    if (sfile != NULL) {
        MemoryStrcat(mm, &f, sfile);
    } else {
        MemoryStrcpy(mm, &src, file);
        MemoryStrcat(mm, &src, "/build");
        Mkdirss(tmp->m, src->m, 0);
        system(tmp->m);
        MemoryStrcat(mm, &src, "/include");
        memset(tmp->m, 0, tmp->size);
        Mkdirss(tmp->m, src->m, 0);
        system(tmp->m);
    }

    DIR *dir = opendir(f->m), *libdir;
    struct dirent *ptr, *libptr;
    FILE *filec, *filebuildc;
    char *ctmp, *ttmp, *nametmp;
    int k;
    while ((ptr = readdir(dir)) != NULL) {
        nametmp = strstr(ptr->d_name, ".");
        if (nametmp != NULL) {
            if (strcmp(nametmp, ".h") == 0) {
                if (sfile == NULL) {
                    MemoryStrcpy(mm, &bin, file);
                    MemoryStrcat(mm, &bin, "/build/include/");
                    MemoryStrcat(mm, &bin, ptr->d_name);

                    MemoryStrcpy(mm, &src, file);
                    MemoryStrcat(mm, &src, "/");
                    MemoryStrcat(mm, &src, ptr->d_name);
                } else {
                    MemoryStrcpy(mm, &bin, file);
                    MemoryStrcat(mm, &bin, "/build/include");
                    MemoryStrcat(mm, &bin, sfile);
                    
                    memset(tmp->m, 0, tmp->size);
                    Mkdirss(tmp->m, bin->m, Linux);
                    system(tmp->m);

                    MemoryStrcat(mm, &bin, "/");
                    MemoryStrcat(mm, &bin, ptr->d_name);

                    MemoryStrcpy(mm, &src, file);
                    MemoryStrcat(mm, &src, sfile);
                    MemoryStrcat(mm, &src, "/");
                    MemoryStrcat(mm, &src, ptr->d_name);
                }

                filebuildc = fopen(bin->m, "w+");
                filec = fopen(src->m, "r");                
                //ctmp = NULL;
                while (fgets(tmp->m, 2048, filec) != NULL) {
			/*if (strstr(tmp->m, "#include \"") != NULL) {
                    strtok_r(tmp->m, "\"", &ctmp);
                    ttmp = strtok_r(NULL, "\"", &ctmp);
                    MemoryStrcpy(mm, &inm, ttmp);
                    strtok_r(ttmp, "/", &ctmp);

                    libdir = opendir(file);
                    while ((libptr = readdir(libdir)) != NULL) {
                    if (strstr(libptr->d_name, ttmp) != NULL) {
                    fputs("#include \"", filebuildc);
                    fputs("include/", filebuildc);
                    fputs(inm->m, filebuildc);
                    fputs("\"\n", filebuildc);
                            break;
                         }
                      }
                      closedir(libdir);
                    } else {*/
                    fputs(tmp->m, filebuildc);
                    //}
                }
                fclose(filec);
                fclose(filebuildc);
            }

            if (strcmp(strstr(ptr->d_name, "."), ".c") == 0) {
                if (sfile == NULL) {
                    MemoryStrcpy(mm, &build, file);
                    MemoryStrcat(mm, &build, "/build/");
                    MemoryStrcat(mm, &build, ptr->d_name);

                    MemoryStrcpy(mm, &src, file);
                    MemoryStrcat(mm, &src, "/");
                    MemoryStrcat(mm, &src, ptr->d_name);
                } else {
                    MemoryStrcpy(mm, &build, file);
                    MemoryStrcat(mm, &build, "/build");
                    MemoryStrcat(mm, &build, sfile);

                    memset(tmp->m, 0, tmp->size);
                    Mkdirss(tmp->m, build->m, 0);
                    system(tmp->m);

                    MemoryStrcat(mm, &build, "/");
                    MemoryStrcat(mm, &build, ptr->d_name);

                    MemoryStrcpy(mm, &src, file);
                    MemoryStrcat(mm, &src, sfile);
                    MemoryStrcat(mm, &src, "/");
                    MemoryStrcat(mm, &src, ptr->d_name);
                }

                filebuildc = fopen(build->m, "w+");
                filec = fopen(src->m, "r");
if (sfile != NULL) {
		memset(tmp->m, 0, tmp->size);
		MemoryStrcat(mm, &tmp, "#include \"../include");
		MemoryStrcat(mm, &tmp, sfile);
		MemoryStrcat(mm, &tmp, "/");
		MemoryStrcat(mm, &tmp, ptr->d_name);
		((char*)tmp->m)[strlen((char*)tmp->m)-1]='h';
		MemoryStrcat(mm, &tmp, "\"\n");
		fputs(tmp->m, filebuildc);
		}   
                k = 0;
                while (fgets(tmp->m, 2048, filec) != NULL) {
                    if (strstr(tmp->m, "#include \"") != NULL) {
                        strtok_r(tmp->m, "\"", &ctmp);
                        ttmp = strtok_r(NULL, "\"", &ctmp);
                        MemoryStrcpy(mm, &inm, ttmp);
                        strtok_r(ttmp, "/", &ctmp);

                        libdir = opendir(file);
                        while ((libptr = readdir(libdir)) != NULL) {
                            if (strstr(libptr->d_name, ttmp) != NULL) {
                                fputs("#include \"", filebuildc);
                                fputs("include/", filebuildc);
                                fputs(inm->m, filebuildc);
                                fputs("\"\n", filebuildc);
                                break;
                            }
                        }
                        closedir(libdir);
                    } else {
                        if (flist != NULL && strstr(tmp->m, "//MemoryFree") != NULL) {
                            MemoryStrcpy(mm, &inm, tmp->m);
                            char *a1 = strtok_r(inm->m, "=", &ctmp);
                            char *b = strtok_r(strtok_r(strtok_r(NULL, "=", &ctmp), ";", &ctmp), "(", &ctmp);
                            char *c = strtok_r(NULL, "(", &ctmp);
                            c[strlen(c) - 1] = '\0';
                            strtok_r(a1, "*", &ctmp);
                            char *t = strtok_r(NULL, "*", &ctmp);

                            char *t1 = strtok_r(c, ",", &ctmp);
                            MemoryInfo *p = MTMemoryManagerCalloc(mm, strlen(t1) + 1);
                            while (t1 != NULL) {
                                MemoryStrcpy(mm, &p, t1);
                                MArrayListAddIndex(mm, plist, p);
                                t1 = strtok_r(NULL, ",", &ctmp);
                            }
                            for (int i = 0; i < MArrayListSize(flist); ++i) {
                                AutoFree *af = (AutoFree *) MArrayListGetIndex(flist, i);
                                for (int j = 0; j < MArrayListSize(af->p1); ++j) {
                                    if (strstr(b, af->ff1->m) != NULL) {
                                        MemoryStrcat(mm, &autofreemem, af->ff2->m);
                                        MemoryStrcat(mm, &autofreemem, "(");
                                        for (int j1 = 0; j1 < MArrayListSize(af->p2); ++j1) {
                                            MemoryInfo *a = MArrayListGetIndex(af->p2, j1);
                                            if (!strcmp(a->m, "memory")) {
                                                MemoryStrcat(mm, &autofreemem, t);
                                            } else {
                                                for (int j2 = 0; j2 < MArrayListSize(af->p1); ++j2) {
                                                    MemoryInfo *mp = MArrayListGetIndex(af->p1, j2);
                                                    if (strcmp(a->m, mp->m)) {
                                                        MemoryStrcat(mm, &autofreemem,
                                                                     ((MemoryInfo *) MArrayListGetIndex(plist, j2))->m);
                                                        break;
                                                    }
                                                }
                                            }
                                            if ((j + 1) != MArrayListSize(af->p2)) {
                                                MemoryStrcat(mm, &autofreemem, ",");
                                            }
                                        }
                                        MemoryStrcat(mm, &autofreemem, ");\n");
                                    }
                                }
                            }
                        }

                        if (strstr(tmp->m, "{") != NULL) {
                            k++;
                        }
                        if (strstr(tmp->m, "}") != NULL) {
                            k--;
                            if (!k) {
                                fputs(autofreemem->m, filebuildc);
                                memset(autofreemem->m, 0, autofreemem->size);
                            }
                        }
                        fputs(tmp->m, filebuildc);
                    }
                }
                for (int i = 0; i < MArrayListSize(plist); ++i) {
                    MTMemoryManagerFree(mm, (MemoryInfo *) MArrayListGetIndex(plist, i));
                }
                plist->length = 0;
                MemoryInfo *cdir = MTMemoryManagerCalloc(mm, strlen(build->m) + 1);
                MemoryStrcpy(mm, &cdir, build->m);
                MArrayListAddIndex(mm, clist, cdir);
                fclose(filec);
                fclose(filebuildc);
            }
        } else {
            if (strcmp(ptr->d_name, "build") &&
                strcmp(ptr->d_name, "library") &&
                strcmp(ptr->d_name, "RTMake") && strcmp(ptr->d_name, "AutoFree")) {
                if (sfile == NULL) {
                    sfilem = MTMemoryManagerCalloc(mm, 1024);
                    MemoryStrcat(mm, &sfilem, "/");
                    MemoryStrcat(mm, &sfilem, ptr->d_name);
                    Pretreatment(mm, file, sfilem->m, clist, flist);
                } else {
                    strcat(sfile, "/");
                    strcat(sfile, ptr->d_name);
                    Pretreatment(mm, file, sfile, clist, flist);
                }
            }
        }
    }
    closedir(dir);
    MTMemoryManagerFree(mm, f);
    MTMemoryManagerFree(mm, build);
    MTMemoryManagerFree(mm, bin);
    MTMemoryManagerFree(mm, src);
    MTMemoryManagerFree(mm, tmp);
    MTMemoryManagerFree(mm, inm);
    MTMemoryManagerFree(mm, autofreemem);
    MArrayListDestroy(mm, plist);
    if (sfilem != NULL) {
        MTMemoryManagerFree(mm, sfilem);
    }
    return clist;
}

MArrayList *ClistGet(MTMemoryManager *mm, char *file, MemoryInfo *sfile, MArrayList *cl) {
    MArrayList *clist = cl;
    if (clist == NULL) {
        clist = MArrayListInit(mm);
    }
    MemoryInfo *f = MTMemoryManagerCalloc(mm, 1024);
    MemoryInfo *build = MTMemoryManagerCalloc(mm, 1024);

    MemoryInfo *sfilem = NULL;

    MemoryStrcpy(mm, &f, file);
    if (sfile != NULL) {
        MemoryStrcat(mm, &f, sfile->m);
    }
    DIR *dir = opendir(f->m);
    struct dirent *ptr;
    while ((ptr = readdir(dir)) != NULL) {
        if (strstr(ptr->d_name, ".c") != NULL) {
            if (sfile == NULL) {
                MemoryStrcpy(mm, &build, file);
                MemoryStrcat(mm, &build, "/build/");
                MemoryStrcat(mm, &build, ptr->d_name);
            } else {
                MemoryStrcpy(mm, &build, file);
                MemoryStrcat(mm, &build, "/build");
                MemoryStrcat(mm, &build, sfile->m);
                MemoryStrcat(mm, &build, "/");
                MemoryStrcat(mm, &build, ptr->d_name);
            }

            MemoryInfo *cdir = MTMemoryManagerCalloc(mm, strlen(build->m) + 1);
            MemoryStrcpy(mm, &cdir, build->m);
            MArrayListAddIndex(mm, clist, cdir);
        }
        if (strstr(ptr->d_name, ".") == NULL && strcmp(ptr->d_name, "build") && strcmp(ptr->d_name, "library") &&
            strcmp(ptr->d_name, "RTMake") && strcmp(ptr->d_name, "AutoFree")) {
            if (sfile == NULL) {
                sfilem = MTMemoryManagerCalloc(mm, 2048);
                MemoryStrcat(mm, &sfilem, "/");
                MemoryStrcat(mm, &sfilem, ptr->d_name);
                ClistGet(mm, file, sfilem, clist);
            } else {
                MemoryStrcat(mm, &sfile, "/");
                MemoryStrcat(mm, &sfile, ptr->d_name);
                ClistGet(mm, file, sfile, clist);
            }
        }
    }
    closedir(dir);
    MTMemoryManagerFree(mm, f);
    MTMemoryManagerFree(mm, build);
    if (sfilem != NULL) {
        MTMemoryManagerFree(mm, sfilem);
    }
    return clist;
}


MemoryInfo *CCompile(MTMemoryManager *mm, int gccorclang, char *file, int type, char *diy, int o, int os) {
    MemoryInfo *c = MTMemoryManagerCalloc(mm, 2048);
    if (os == 0) {
        if (type == 0) {
            MemoryStrcpy(mm, &c, "cd ");
            MemoryStrcat(mm, &c, file);
            MemoryStrcat(mm, &c, "/build;");
            if (gccorclang) {
                MemoryStrcat(mm, &c, "gcc ");
            } else {
                MemoryStrcat(mm, &c, "clang ");
            }
            MemoryInfo *tmp = MTMemoryManagerCalloc(mm, 1024);
            MemoryInfo *tmp1 = MTMemoryManagerCalloc(mm, 1024);

            MemoryStrcpy(mm, &tmp, file);
            MemoryStrcat(mm, &tmp, "/build/bin");
            Mkdirss(tmp1->m, tmp->m, os);
            system(tmp1->m);

            MemoryStrcpy(mm, &tmp, file);
            MemoryStrcat(mm, &tmp, "/build");
            DIR *dldir = opendir(tmp->m);
            struct dirent *dlptr;
            while ((dlptr = readdir(dldir)) != NULL) {
                if (strstr(dlptr->d_name, ".o") != NULL) {
                    MemoryStrcat(mm, &c, dlptr->d_name);
                    MemoryStrcat(mm, &c, " ");
                }
            }
            closedir(dldir);

            MemoryStrcpy(mm, &tmp, file);
            MemoryStrcat(mm, &tmp, "/library");
            dldir = opendir(tmp->m);
            while ((dlptr = readdir(dldir)) != NULL) {
                if (strstr(dlptr->d_name, ".so") != NULL) {
                    MemoryStrcpy(mm, &tmp, "cp -r ");
                    MemoryStrcat(mm, &tmp, file);
                    MemoryStrcat(mm, &tmp, "/library ");
                    MemoryStrcat(mm, &tmp, file);
                    MemoryStrcat(mm, &tmp, "/build/bin");
                    system(tmp->m);
                    MemoryStrcat(mm, &c, "-Wl,-rpath=\"library\" ");
                    break;
                }
                if (strstr(dlptr->d_name, ".a") != NULL) {
                    MemoryStrcpy(mm, &tmp, "cp -r ");
                    MemoryStrcat(mm, &tmp, file);
                    MemoryStrcat(mm, &tmp, "/library ");
                    MemoryStrcat(mm, &tmp, file);
                    MemoryStrcat(mm, &tmp, "/build/bin");
                    system(tmp->m);
                    MemoryStrcat(mm, &c, "-Wl,-rpath=\"library\" ");
                    break;
                }
            }
            closedir(dldir);

            MemoryStrcat(mm, &c, "-o bin/run -I ");
            MemoryStrcat(mm, &c, file);
            MemoryStrcat(mm, &c, " -L ");
            MemoryStrcat(mm, &c, file);
            MemoryStrcat(mm, &c, "/library ");
            if (o == 1) {
                MemoryStrcat(mm, &c, "-O1 ");
            }
            if (o == 2) {
                MemoryStrcat(mm, &c, "-O2 ");
            }
            if (o == 3) {
                MemoryStrcat(mm, &c, "-O3 ");
            }

            MemoryStrcpy(mm, &tmp, file);
            MemoryStrcat(mm, &tmp, "/library/config");
            FILE *con = fopen(tmp->m, "r");
            char a[1024];
            while (fgets(a, 1024, con) != NULL) {
                MemoryStrcat(mm, &c, "-l ");
                a[strlen(a) - 1] = '\0';
                MemoryStrcat(mm, &c, a);
                MemoryStrcat(mm, &c, " ");
            }
            fclose(con);

            if (diy != NULL) {
                MemoryStrcat(mm, &c, diy);
            }

            MTMemoryManagerFree(mm, tmp);
            MTMemoryManagerFree(mm, tmp1);
        }
        if (type == 1) {
            MemoryStrcpy(mm, &c, "cd ");
            MemoryStrcat(mm, &c, file);
            MemoryStrcat(mm, &c, "/build;");
            if (gccorclang) {
                MemoryStrcat(mm, &c, "gcc ");
            } else {
                MemoryStrcat(mm, &c, "clang ");
            }
            MemoryInfo *tmp = MTMemoryManagerCalloc(mm, 1024);
            MemoryInfo *tmp1 = MTMemoryManagerCalloc(mm, 1024);

            MemoryStrcpy(mm, &tmp, file);
            MemoryStrcat(mm, &tmp, "/build/bin");
            Mkdirss(tmp1->m, tmp->m, os);
            system(tmp1->m);

            MemoryStrcpy(mm, &tmp, file);
            MemoryStrcat(mm, &tmp, "/build");
            DIR *dldir = opendir(tmp->m);
            struct dirent *dlptr;
            while ((dlptr = readdir(dldir)) != NULL) {
                if (strstr(dlptr->d_name, ".o") != NULL) {
                    MemoryStrcat(mm, &c, dlptr->d_name);
                    MemoryStrcat(mm, &c, " ");
                }
            }
            closedir(dldir);

            MemoryStrcpy(mm, &tmp, file);
            MemoryStrcat(mm, &tmp, "/library");
            dldir = opendir(tmp->m);
            while ((dlptr = readdir(dldir)) != NULL) {
                if (strstr(dlptr->d_name, ".so") != NULL) {
                    MemoryStrcpy(mm, &tmp, "cp -r ");
                    MemoryStrcat(mm, &tmp, file);
                    MemoryStrcat(mm, &tmp, "/library ");
                    MemoryStrcat(mm, &tmp, file);
                    MemoryStrcat(mm, &tmp, "/build/bin && cd build/bin/library && rm -f config");
                    system(tmp->m);
                    MemoryStrcat(mm, &c, "-Wl,-rpath=\"library\" ");
                    break;
                }
                if (strstr(dlptr->d_name, ".a") != NULL) {
                    MemoryStrcpy(mm, &tmp, "cp -r ");
                    MemoryStrcat(mm, &tmp, file);
                    MemoryStrcat(mm, &tmp, "/library ");
                    MemoryStrcat(mm, &tmp, file);
                    MemoryStrcat(mm, &tmp, "/build/bin && cd build/bin/library && rm -f config");
                    system(tmp->m);
                    MemoryStrcat(mm, &c, "-Wl,-rpath=\"library\" ");
                    break;
                }
            }
            closedir(dldir);

            MemoryStrcat(mm, &c, "-shared -fPIC -o bin/lib.so -I ");
            MemoryStrcat(mm, &c, file);
            MemoryStrcat(mm, &c, " -L ");
            MemoryStrcat(mm, &c, file);
            MemoryStrcat(mm, &c, "/library ");
            if (o == 1) {
                MemoryStrcat(mm, &c, "-O1 ");
            }
            if (o == 2) {
                MemoryStrcat(mm, &c, "-O2 ");
            }
            if (o == 3) {
                MemoryStrcat(mm, &c, "-O3 ");
            }

            MemoryStrcpy(mm, &tmp, file);
            MemoryStrcat(mm, &tmp, "/library/config");
            FILE *con = fopen(tmp->m, "r");
            char a[1024];
            while (fgets(a, 1024, con) != NULL) {
                MemoryStrcat(mm, &c, "-l ");
                a[strlen(a) - 1] = '\0';
                MemoryStrcat(mm, &c, a);
                MemoryStrcat(mm, &c, " ");
            }
            fclose(con);

            if (diy != NULL) {
                MemoryStrcat(mm, &c, diy);
            }

            MTMemoryManagerFree(mm, tmp);
            MTMemoryManagerFree(mm, tmp1);
        }
        if (type == 2) {
            MemoryStrcpy(mm, &c, "cd ");
            MemoryStrcat(mm, &c, file);
            MemoryStrcat(mm, &c, "/build;ar -r bin/lib.a ");

            MemoryInfo *tmp = MTMemoryManagerCalloc(mm, 1024);
            MemoryInfo *tmp1 = MTMemoryManagerCalloc(mm, 1024);

            MemoryStrcpy(mm, &tmp, file);
            MemoryStrcat(mm, &tmp, "/build/bin");
            Mkdirss(tmp1->m, tmp->m, os);
            system(tmp1->m);

            MemoryStrcpy(mm, &tmp, file);
            MemoryStrcat(mm, &tmp, "/build");
            DIR *dldir = opendir(tmp->m);
            struct dirent *dlptr;
            while ((dlptr = readdir(dldir)) != NULL) {
                if (strstr(dlptr->d_name, ".o") != NULL) {
                    MemoryStrcat(mm, &c, dlptr->d_name);
                    MemoryStrcat(mm, &c, " ");
                }
            }
            closedir(dldir);

            MTMemoryManagerFree(mm, tmp);
            MTMemoryManagerFree(mm, tmp1);
        }
    }
    system(c->m);
    return c;
}

int RTMakeAllO(MTMemoryManager *mm, char *file, MArrayList *listc, int o) {
    MemoryInfo *comlilem = MTMemoryManagerCalloc(mm, 1024);
    MemoryStrcpy(mm, &comlilem, "cd ");
    MemoryStrcat(mm, &comlilem, file);
    MemoryStrcat(mm, &comlilem, "/build");
    MemoryStrcat(mm, &comlilem, ";clang ");
    for (int i = 0; i < MArrayListSize(listc); ++i) {
        //printf("info:%s\n",((MemoryInfo *) MArrayListGetIndex(listc, i))->m);
        if (((MemoryInfo *) MArrayListGetIndex(listc, i))->m != NULL) {
            MemoryStrcat(mm, &comlilem, ((MemoryInfo *) MArrayListGetIndex(listc, i))->m);
            MemoryStrcat(mm, &comlilem, " ");
        }
    }
    MemoryStrcat(mm, &comlilem, "-c -fPIC");
    if (o == 1) {
        MemoryStrcat(mm, &comlilem, " -O1");
    }
    if (o == 2) {
        MemoryStrcat(mm, &comlilem, " -O2");
    }
    if (o == 3) {
        MemoryStrcat(mm, &comlilem, " -O3");
    }

    int status;
    status = system(comlilem->m);
    MTMemoryManagerFree(mm, comlilem);
    return status;
}

void installlib(MTMemoryManager *mm, char *path, char *name) {
    MemoryInfo *comlilem = MTMemoryManagerCalloc(mm, 1024);
    MemoryStrcpy(mm, &comlilem, "sudo cp -r ");
    MemoryStrcat(mm, &comlilem, path);
    MemoryStrcat(mm, &comlilem, "/build/include /usr/include/");
    MemoryStrcat(mm, &comlilem, name);
    MemoryStrcat(mm, &comlilem, "/include/;sudo cp -f ");
    MemoryStrcat(mm, &comlilem, path);
    MemoryStrcat(mm, &comlilem, "/build/bin/lib.so /usr/lib/lib");
    MemoryStrcat(mm, &comlilem, name);
    MemoryStrcat(mm, &comlilem, ".so");
    system(comlilem->m);
    MTMemoryManagerFree(mm, comlilem);
}
