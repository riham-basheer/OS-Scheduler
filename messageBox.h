#ifndef OS_SCHEDULER_MESSAGEBOX_H
#define OS_SCHEDULER_MESSAGEBOX_H
#include <sys/msg.h>
#include <signal.h>
#include "processData.h"
#include "def.h"
/* Message Box: controls all that is related to the message queue used to send
processes from the process generator to the scheduler. */
#define MESSAGEBOX_KEY 12345
#define PERMISSIONS 0644
#define EOT_TYPE 9L

int qID; //

struct messageBuffer
{
    long mtype;
    processData data;
};

/* Creates the message queue with the defined key. */
void newMessageBox()
{

    qID = msgget(MESSAGEBOX_KEY, IPC_CREAT | PERMISSIONS);
    if (qID == -1)
        perror("Message Box created successfully.\n");
}

/* Connects to the created message queue in the receiving end.*/
void connectToMessageBox()
{
    qID = msgget(MESSAGEBOX_KEY, 0666);
    if (qID == -1)
    {
        kill(getpgrp(), SIGINT);
    }
}

/* send */
bool sendMessage(processData pData)
{
    struct messageBuffer sms;
    sms.mtype = 1L;
    sms.data = pData;

    int val = msgsnd(qID, &sms, sizeof(sms.data), !IPC_NOWAIT);
    if (val == 0)
        return (val == 0);
}

void sendEOT()
{
    struct messageBuffer eot;
    eot.mtype = EOT_TYPE;
    msgsnd(qID, &eot, sizeof(eot.data) - sizeof(long), !IPC_NOWAIT);
}

void deleteMessageBox()
{
    msgctl(qID, IPC_RMID, NULL);
    exit(0);
}

int recvMessage(processData *pData)
{
    struct messageBuffer sms;
    sms.mtype = 1L;
    int val = msgrcv(qID, &sms, sizeof(sms.data), 0, IPC_NOWAIT);
    *pData = sms.data;
    if (val == -1)
        return -1;
    if (sms.mtype == EOT_TYPE)
    {
        printf("\nOK WE SHOULD LEAVE\n");
        return 1;
    }
    return 0;
}

#endif