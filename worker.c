#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define SHMKEY  960158     /* Parent and child agree on common key.*/
#define BUFF_SZ sizeof ( int )

//message buffer from https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_message_queues.htm
#define PERMS 0644
struct my_msgbuf {
   long mtype;
   char mtext[200];
};

int main(int argc, char** argv)
{
        printf("Worker: We are in the child class\n");
        int shmid = shmget ( SHMKEY, BUFF_SZ, 9045);

        //initializing needed variables for message buffer
        struct my_msgbuf buf;
        int msqid;
        int toend;
        int len;
        key_t key;
        char *msgText = "(Un)Locked and ready to go";
        system("touch msgq.txt");

        //atteching key to shared memory
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

        //intializing variables and setting value of m to passed parameter m from parent
        int m = atoi(argv[1]);
        int x;
        int i;
        printf("Worker: the value of m is: %d\n", m);
        //int for what m is after 1000000
        int mAdd = m*1000000;
        //shared memory integers
        char *secClock = (char*) (shmat(SHMKEY, NULL, 9045));
        char *nanoClock = (char*) (shmat(SHMKEY, NULL , 9045));

        int *sharedNanoClock = (int*)(secClock);
        int *sharedSecClock = (int*)(nanoClock);

        printf("Worker: about to enter the for loop\n");
        //for loop to reach m processes
        for(x = 0; x < m; x++)
        {
                /*message receive before critical section
                 *both send and receive functions adapted from
                 *https://www.tutorialspoint.com/inter_process_communication/inter_process_communication_message_queues.htm
                 */
                if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == 0)
                {
                        break;
                }
                else if (msgrcv(msqid, &buf, sizeof(buf.mtext), 0, 0) == -1)
                {
                        perror("msgrcv");
                        exit(1);
                }
                sleep(1);

                printf("Worker: Entering the for loop\n");
                //adds the million value to nanoClock
                //was breaking program and cannot fix so it's commented out
                nanoClock += mAdd;

                printf("Worker: added to clock loop\n");

                //worker prints its pid, iteration number, and increment amount
                                printf("Worker: %d",getpid());
                printf(": Iteration %d:", m);
                printf("  incrementing by %d\n", mAdd);
                printf("Worker: The clock is at %d", sharedNanoClock);
                printf(" and %d seconds\n", sharedSecClock);

                if ((*sharedNanoClock >= 1000000000) && (*sharedNanoClock < 2000000000))
                {
                        *sharedNanoClock -= 1000000000;
                        *sharedSecClock++;
                }
                //in case nanoClock has somehow gone over 2000000000
                else if (*sharedNanoClock >= 2000000000)
                {
                        //creates loop to increase second clock and decrese nanoClock
                        while(*sharedNanoClock >= 1000000000)
                        {
                                *sharedNanoClock -= 1000000000;
                                *sharedSecClock++;
                        }
                }
                sleep(1);
                //sending message after critical section
                buf.mtype = 1;
                strcpy(buf.mtext, msgText);
                len = strlen(buf.mtext);
                if(msgsnd(msqid, &buf, len+1, 0) == -1)
                {
                        perror("msgsnd");
                }

        }

        printf("\nWorker is now ending.\n");
        //freeing memory
        shmdt(secClock);
        shmdt(nanoClock);
        shmdt(sharedSecClock);
        shmdt(sharedNanoClock);
        sleep(3);
        return EXIT_SUCCESS;
}
