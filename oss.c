#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/ipc.h>

//shared memory
#define SHMKEY  960158     /* Parent and child agree on common key.*/
#define BUFF_SZ sizeof ( int )

//message queue struct from https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_message_queues.htm
#define PERMS 0644
struct my_msgbuf {
   long mtype;
   char mtext[200];
};


//using form provided OS_tutorial
void print_usage ( const char * app )
{
        fprintf (stderr, "usage: %s [-h] [-n maxChildren] [-s existChildren] [-m increment]\n",app);
        fprintf (stderr, "    maxChildrem is max of children created total\n");
        fprintf (stderr, "    existChildren is max children at one time\n");
        fprintf (stderr, "    increment is the amount children increment clock \n\n");
}


//main body
int main(int argc, char** argv)
{

        //message queue
        //https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_message_queues.htm
        struct my_msgbuf buf;
        int msqid;
        int len;
        key_t key;
        system("touch msgq.txt");

        int q;
        int w;

        q = atoi(argv[1]);
        w = atoi(argv[2]);



        printf("Arguments: %d\n", argv[0]);
        printf("Arguments: %d\n", q);
        printf("Arguments: %d\n", w);


        if ((key = ftok("msgq.txt", 'B')) == -1)
        {
                perror("ftok");
                exit(1);
        }
        if ((msqid = msgget(key, PERMS | IPC_CREAT)) == -1)
        {
                perror("msgget");
                exit(1);
        }
        printf("OSS: Message Queue: Set up and ready to send\n");
        buf.mtype = 1;


        //CREATION OF SHARED MEMORY
        int shmid = shmget ( SHMKEY, BUFF_SZ, 9045 | IPC_CREAT );

        char *secClock = (char*) shmat(shmid,NULL, 0);
        char *nanoClock = (char*) shmat(shmid,NULL, 0);

        int *sharedClockSec = (int*) (secClock);
        int *sharedClockNano = (int*) (nanoClock);

        int i;
        int j;

}
