#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/types.h>
void sem_unlock(int semid, int num)
{
    struct sembuf sem_op;
    sem_op.sem_num = num;
    sem_op.sem_op = 1;
    sem_op.sem_flg = 0;
    semop(semid, &sem_op, 1);
}
void sem_lock(int semid, int num)
{

    struct sembuf sem_op;
    sem_op.sem_num = num;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;
    semop(semid, &sem_op, 1);
    if (num == 0)
    {
        printf("\n");
    }
}
int main()
{
    int sem_id = semget(IPC_PRIVATE, 4, 0777 | IPC_CREAT);
    semctl(sem_id, 0, SETVAL, 1);
    semctl(sem_id, 1, SETVAL, 0);
    semctl(sem_id, 2, SETVAL, 0);
    semctl(sem_id, 3, SETVAL, 0);
    int pid = fork();
    if (pid == 0)
    {
        int pid2 = fork();
        if (pid2 != 0)
        {
            while (1)
            {
                sem_lock(sem_id, 0);
                int c = rand() % 3;
                printf("Agent process");
                sleep(7);
                switch (c)
                {
                case 0:
                    sem_unlock(sem_id, 1);
                    break;
                case 1:
                    sem_unlock(sem_id, 2);
                    break;
                case 2:
                    sem_unlock(sem_id, 3);
                    break;
                }
                sleep(1);
            }
        }
        else
        {
            sleep(3);
            while (1)
            {
                sem_lock(sem_id, 1);
                printf("SMoker 1\n");
                printf("I made the cigarette\n");
                sleep(1);
                sem_unlock(sem_id, 0);
            }
        }
    }
    else
    {
        int pid3 = fork();
        if (pid3 == 0)
        {
            sleep(3);
            while (1)
            {
                sem_lock(sem_id, 2);
                printf("Smoker 2\n");
                printf("I made the cigarette\n");
                sleep(1);
                sem_unlock(sem_id, 0);
            }
        }
        else
        {

            sleep(3);
            while (1)
            {
                sem_lock(sem_id, 3);
                printf("Smoker 3\n");
                printf("I made the cigarette\n");
                sleep(1);
                sem_unlock(sem_id, 0);
            }
        }
    }
    return 0;
