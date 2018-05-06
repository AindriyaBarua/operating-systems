#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>

#define BUFFER_SIZE 5
#define EMPTY_ID 0
#define FULL_ID 1
#define MUTEX_ID 2
#define NSEM_SIZE 3
#define SHM_KEY 9
#define SEM_KEY "."

static struct sembuf downEmpty = { EMPTY_ID, -1, 0 };
static struct sembuf upEmpty = { EMPTY_ID, 1, 0 };
static struct sembuf upFull = { FULL_ID, 1, 0 };
static struct sembuf downFull = { FULL_ID, -1, 0 };
static struct sembuf downMutex = { MUTEX_ID, -1, 0 };
static struct sembuf upMutex = { MUTEX_ID, 1, 0 };


int *create_shared_mem_buffer();
int create_semaphore_set();
int get_buffer_size(int *sbuff);
void clear_buffer(int *sbuf);

int main(int argc, char **argv)
{	
	int pid=fork();
	if(pid==0){
		int *shared_buffer = create_shared_mem_buffer();
		int semid = create_semaphore_set();
		int item = 0;

		while(1) {
			semop(semid, &downFull, 1);
			semop(semid, &downMutex, 1);
			item = remove_item(semid, shared_buffer);
			debug_buffer(shared_buffer);
			semop(semid, &upMutex, 1);
			semop(semid, &upEmpty, 1);
			consume_item(item);
		} 
		return EXIT_SUCCESS;
	}
	else{
		int *shared_buffer = create_shared_mem_buffer();
		int semid = create_semaphore_set();
		clear_buffer(shared_buffer); 
		int item = 0;

		while(1) {
			item = produce_item();
			semop(semid, &downEmpty, 1);
			semop(semid, &downMutex, 1);
			insert_item(item, semid, shared_buffer);
			debug_buffer(shared_buffer);
			semop(semid, &upMutex, 1);
			semop(semid, &upFull, 1);
		}
		return EXIT_SUCCESS;
	}
	
	return 0;
}
int create_semaphore_set(){
	key_t key = ftok(SEM_KEY, 'E');
	int semid = semget(key, NSEM_SIZE, 0600 | IPC_CREAT);
	if (semid == -1) {
		perror("semget");
		exit(1);
	}
	  semctl(semid, FULL_ID, SETVAL, 0);
	  if (errno > 0) {
		perror("failed to set FULL semaphore");
		exit (EXIT_FAILURE);
	  }
	semctl(semid, EMPTY_ID, SETVAL, BUFFER_SIZE);
	if (errno > 0) {
		perror("failed to set EMPTY sempahore");
		exit (EXIT_FAILURE);
	}
	semctl(semid, MUTEX_ID, SETVAL, 1);
	if (errno > 0) {
		perror("failed to create mutex");
	}
	return semid;
}
int *create_shared_mem_buffer(){
	int *shmaddr=0;
	int shmid=0;
	shmid=shmget(IPC_PRIVATE, 100*sizeof(int), 0777|IPC_CREAT);
	if (errno > 0) {
		perror("failed to create shared memory segment");
		exit (EXIT_FAILURE);
	}
	shmaddr = (int*)shmat(shmid, NULL, 0);
	if (errno > 0) {
		perror ("failed to attach to shared memory segment");
		exit (EXIT_FAILURE);
	}
	return shmaddr;
}
void clear_buffer(int *sbuff) {
  int i = 0;
  for (i = 0; i < BUFFER_SIZE; ++i){
	   sbuff[i] = 0x00;
   }
}
int get_buffer_size(int *sbuff) {
  int i = 0;
  int counter = 0;
  for (i = 0; i < BUFFER_SIZE; ++i) {
    if (sbuff[i] != 0x00) {
      counter++;
    } 
  }
  return counter;
}

void debug_buffer(int *sbuff) {
  int i = 0;
  for (i = 0; i < BUFFER_SIZE; ++i) {
    if (sbuff[i] == 0xFF) printf("1");
  }
  printf("\n");
}
void insert_item(int item, int semid, int *shared_buffer) {
  int index = get_buffer_size(shared_buffer);
  shared_buffer[index] = item; 
}
int produce_item() {
  return 0xFF; 
}
void consume_item(int item) {
}
int remove_item(int semid, int *shared_buffer) {
  int index = get_buffer_size(shared_buffer) - 1;
  int item = shared_buffer[index];
  shared_buffer[index] = 0x00;
  return item;
}

