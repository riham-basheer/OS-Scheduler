#include <sys/msg.h>
#include <signal.h>
#include "processData.h"
#include "def.h"

#define MESSAGEBOX_KEY 12345
#define PERMISSIONS 0644
#define EOT_TYPE 10

int qID;

struct messageBuffer
{
    long mtype;
    processData data;
};

void newMessageBox()
{
    qID = msgget(MESSAGEBOX_KEY, IPC_CREAT | PERMISSIONS);
    if (qID == -1)
        perror("Message Box created successfully.\n");
}

bool sendMessage(processData *pData)
{
    struct messageBuffer sms;
    sms.mtype = 1;
    int val = msgsnd(qID, &sms, sizeof(sms.data), !IPC_NOWAIT);
    return (val == 0);
}

void sendEOT()
{
    struct messageBuffer eot;
    eot.mtype = EOT_TYPE;
    msgsnd(qID, &eot, sizeof(eot.data), !IPC_NOWAIT);
}

void deleteMessageBox()
{
    msgctl(qID, IPC_RMID, NULL);
    exit(0);
}

bool recvMessage(processData *pData)
{
    struct messageBuffer sms;
    int val = msgrcv(qID, &sms, sizeof(sms), 0, IPC_NOWAIT);
    *pData = sms.data;
    if (val == -1)
    {
        perror("\nFailed to receive message.\n");
        exit(7);
    }
    if (val == EOT_TYPE)
        return false;
    return true;
}