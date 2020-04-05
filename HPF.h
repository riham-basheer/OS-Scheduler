#ifndef OS_SCHEDULER_HPF_H
#define OS_SCHEDULER_HPF_H

#include "PCB.h"
#include "processData.h"
#include "priorityQueue.h"
#include "messageBox.h"
#include "headers.h"
#include <signal.h>
#include <stdio.h>


void HPF()
{
    priorityQueue *queue = new_PrioriyQueue();
    connectToMessageBox();
    processData process;
    int recValue = recvMessage(&process);
    while (!priority_isEmpty(queue) || recValue != 1)
    {
        while (recValue== 0)
        {
            printf("\nReceived a process now. It's # %d\n", process.id);
            PCB currProcess = create_PCB(&process);
            priority_enqueue(queue, &currProcess);
            recValue = recvMessage(&process);
        }

        PCB processToRun;
        priority_dequeue(queue, &processToRun);
        int pid_process = create_process(processToRun.remainingTime);
        processToRun.status = STARTED;
        printf("At time %d process started arr %d total %d remain %d wait %d\n",
               getClk(), processToRun.processStruct.id, processToRun.processStruct.arrivaltime,
               processToRun.processStruct.runningtime, processToRun.remainingTime, 0);
        processToRun.startTime = getClk();
        int STATUS;
        waitpid(pid_process, &STATUS, 0);
        processToRun.TA = getClk() - processToRun.startTime;
        processToRun.remainingTime = 0;
        processToRun.status = FINISHED;
    }
}

int create_process(int runTime)
{
    int pid_schd = fork();
    if (pid_schd == -1)
        perror("\nmamamamamam\n");
    else if (pid_schd == 0)
    {
        char str[64];
        sprintf(str, "%d", runTime);
        if (execl("process.out", "process.out", str, NULL) == -1)
        {
            perror("\nCouldn't create the scheduler.\n");
            exit(10);
        }
    }
    else
    {
        return pid_schd;
    }
}

PCB create_PCB(processData *process)
{
    PCB pcb;
    pcb.processStruct.arrivaltime = process->arrivaltime;
    pcb.processStruct.id = process->id;
    pcb.processStruct.priority = process->priority;
    pcb.processStruct.runningtime = process->runningtime;
    pcb.remainingTime = process->runningtime;
    pcb.status = STOPPED;
    return pcb;
}

#endif