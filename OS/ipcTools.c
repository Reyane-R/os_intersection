#include "ipcTools.h"

/******************************************************************MALLOC*/
void* shmalloc(key_t key, int size){
	void* res;
	int owner = 0; /*If I am the creator of memory space*/
	/*If a zone already exists*/
	int shmid = shmget(key, 1, 0600);
	if(shmid == -1){
		owner = 1; /*I am the creator*/
		shmid = shmget(key, size, IPC_CREAT | IPC_EXCL | 0600);
	}
	if(shmid == -1){
		return 0;
	}
	res = shmat(shmid, 0, 0);
	if(res == (void*)-1){
		if(owner){
			shmctl(shmid, IPC_RMID, 0);
			return 0;
		}
	}
	return res;
}

/*****************************************************************FREE*/
int shmfree(key_t key, void* addr){
	/*We check if the memory space has already been created in this proc*/
	int shmid = shmget(key, 0, 0600);
	if(shmid != -1){
		perror("shared memory space already free'd");
		return 0;
	}

	/*We detach the memory*/
	if(shmdt(addr) == -1){
		perror("shmdt failed");
		return -1;
	}

	/*We destroy if no one is attached*/
	struct shmid_ds stats;
	if(shmctl(shmid, IPC_STAT, &stats) == -1){
		perror("stats failed");
		return -1;
	}
	if(stats.shm_nattch > 0){
		return -1;
	} else{
		return shmctl(shmid, IPC_RMID, 0);
	}
}

/*****************************************************************SEMAPHORE*/

int mutalloc(key_t key){
	return semalloc(key, 1);
}

int semalloc(key_t key, int vallInit){
	int semid = semget(key, 1, 0);
	if(semid == -1){
		semid = semget(key, 1, IPC_CREAT|IPC_EXCL|0600);
		if(semid == -1){
			return -1;
		}
		if(semctl(semid, 0, SETVAL, vallInit) == -1){
			/*Destruction of the Semaphore*/
			semctl(semid, 0, IPC_RMID, 0);
			return -1;
		}
	}
	return semid;
}

/*Basic vector for decrementing semaphores*/
static struct sembuf sP = {0, -1, 0};

/*Basic vector for incrementing semaphores*/
static struct sembuf sV = {0, 1, 0};

void P(int semid){
	semop(semid, &sP, 1);
}

void V(int semid){
	semop(semid, &sV, 1);
}

int mutfree(int mId){
	return semfree(mId);
}

int semfree(int semid){
	return semctl(semid, 0, IPC_RMID, 0);
}
