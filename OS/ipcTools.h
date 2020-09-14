#ifndef UTIL
#define UTIL

typedef enum bool bool;
enum bool {false = 0, true = 1};

#endif
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>

void *shmalloc(key_t key, int size);
int shmfree(key_t key, void *shmaddr);

int mutalloc(key_t key);
int semalloc(key_t key, int vallInit);

void P(int semid);
void V(int semid);

int mutfree(int mId);
int semfree(int semid);



