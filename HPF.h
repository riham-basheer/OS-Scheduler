#ifndef OS_SCHEDULER_HPF_H
#define OS_SCHEDULER_HPF_H

#include "PCB.h"
#include "processData.h"
#include "priorityQueue.h"
#include "messageBox.h"
#include <signal.h>
#include <stdio.h>

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

void HPF()
{
    priorityQueue *queue = new_PrioriyQueue();
    connectToMessageBox();
    processData process;
    PCB currProcess, processToRun;
    int recValue = recvMessage(&process);
    bool noMoreProcesses = (recValue == 1);

    while (!priority_isEmpty(queue->head) || !noMoreProcesses)
    {

        while (recValue == 0)
        {
            printf("\nReceived a process now. It's # %d\n", process.id);
            currProcess = create_PCB(&process);
            priority_enqueue(queue, &currProcess);
            recValue = recvMessage(&process);
            if (recValue == 1)
            noMoreProcesses = true;
        }
        if (priority_dequeue(queue, &processToRun))
        {
            int pid_process = create_process(processToRun.remainingTime);
            processToRun.status = STARTED;
            printf("At time %d process %d started arr %d total %d remain %d wait %d\n",
                   getClk(), processToRun.processStruct.id, processToRun.processStruct.arrivaltime,
                   processToRun.processStruct.runningtime, processToRun.remainingTime,
                   getClk() - processToRun.processStruct.arrivaltime);
            processToRun.startTime = getClk();
            int STATUS;
            waitpid(pid_process, &STATUS, 0);
            processToRun.TA = getClk() - processToRun.startTime;
            processToRun.remainingTime = 0;
            processToRun.status = FINISHED;
            printf("At time %d process %d finished arr %d total %d remain %d wait %d finished at %d\n",
                   getClk(), processToRun.processStruct.id, processToRun.processStruct.arrivaltime,
                   processToRun.processStruct.runningtime, processToRun.remainingTime,
                   getClk() - processToRun.processStruct.arrivaltime, processToRun.TA + processToRun.startTime);
        }
        recValue = recvMessage(&process);
    }
    printf("\n********************************\n");
}

#endif