#ifndef OS_SCHEDULER_MESSAGEBOX_H
#define OS_SCHEDULER_MESSAGEBOX_H
#include <sys/msg.h>
#include <signal.h>
#include <stdio.h>
#include "processData.h"
#include "def.h"

/* Message Box: controls all what is related to the message queue used to send
processes from the process generator to the scheduler. */
#define MESSAGEBOX_KEY 12345
#define PERMISSIONS 0644
#define EOT_TYPE 5L

int qID;

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
        perror("Message Box creation\n");
};

/* Connects to the created message queue in the receiving end.*/
void connectToMessageBox()
{
    qID = msgget(MESSAGEBOX_KEY, PERMISSIONS);
    if (qID == -1)
    {
        printf("\nScheduler: Messagebox not connected.\n");
        exit(-1);
    }
};

/* send process. */
bool sendMessage(processData pData)
{
    struct messageBuffer sms;
    sms.mtype = 1L;
    sms.data = pData;

    int val = msgsnd(qID, &sms, sizeof(sms.data), !IPC_NOWAIT);
    return (val == 0);
};

/* send EOT: no more processes to schedule. */
void sendEOT()
{
    struct messageBuffer eot;
    eot.mtype = EOT_TYPE;
    msgsnd(qID, &eot, sizeof(eot.data), !IPC_NOWAIT);
};

/* Remove message box. */
void deleteMessageBox()
{
    msgctl(qID, IPC_RMID, NULL);
    exit(0);
};

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
        printf("\nNo more processes.\n");
        return 1;
    }
    return 0;
};

#endif