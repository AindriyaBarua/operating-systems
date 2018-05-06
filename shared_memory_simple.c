#include <stdio.h> 
#include <sys/ipc.h> 
#include <sys/shm.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <ctype.h>
int main()
{
    int *a,*b;
    int shmid,pid,status;
    shmid=shmget(IPC_PRIVATE,sizeof(int),0777|IPC_CREAT);
    pid=fork();
    if(pid<0){
        exit(0);
    }
    else if(pid==0)
    {
        a=(int*)shmat(shmid,0,0);
        int n;
        printf("Enter the number\n");
        scanf("%d",&n);
        a[0]=n;
        shmdt(a);
    }
    else
    {
        wait(&status);
        b=(int *)shmat(shmid,0,0);
        int x=b[0];
        printf("The Enter number is %d",x);
        shmdt(b);
    }
    return 0;
}