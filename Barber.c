#include <stdio.h>
#include <stdlib.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <sys/types.h>
#define CHAIRS 2

typedef int semaphore;
int waiting = 0;
int customers, barbers, mutex;
void V(int semid)
{
    struct sembuf sem_op;
    sem_op.sem_num = 0;
    sem_op.sem_op = 1;
    sem_op.sem_flg = 0;
    semop(semid, &sem_op, 1);
}
void P(int semid)
{

    struct sembuf sem_op;
    sem_op.sem_num = 0;
    sem_op.sem_op = -1;
    sem_op.sem_flg = 0;
    semop(semid, &sem_op, 1);
}
void barber(void)
{
    while (1)
    {
        P(customers); /*go to sleep if no customers*/
        P(mutex);
        waiting = waiting - 1;
        V(barbers);
        V(mutex);
        printf("barber is cutting hair\n");
    }
}
void customer(int n)
{
    P(mutex);
    if (waiting < CHAIRS)
    {
        waiting = waiting + 1;
        V(customers);
        V(mutex);
        P(barbers);
        printf("Customer %d is getting haircut\n", waiting);
    }
    else
    {
        printf("Customer %d has left due to lack of seat\n", n);
        V(mutex);
    }
}

int main()
{
    customers = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);
    barbers = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);
    mutex = semget(IPC_PRIVATE, 1, 0777 | IPC_CREAT);
    semctl(customers, 0, SETVAL, 0);
    semctl(barbers, 0, SETVAL, 0);
    semctl(mutex, 0, SETVAL, 1);

    int pid = fork();
    if (pid == 0)
    {
        int pid2 = fork();
        if (pid2 != 0)
        {
            barber();
        }
        else
        {
            sleep(3);
            while (1)
            {
                customer(1);
                sleep(2);
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
                customer(2);
                sleep(2);
            }
        }
        else
        {

            sleep(3);
            while (1)
            {
                customer(3);
                sleep(2);
            }
        }
    }
    return 0;
}