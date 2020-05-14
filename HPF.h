#ifndef OS_SCHEDULER_HPF_H
#define OS_SCHEDULER_HPF_H

#include <unistd.h>
#include "priorityQueue.h"
#include "messageBox.h"
#include "outputFile.h"
#include "PCB.h"

int create_process(int runTime)
{
    int pid_prcs = fork();
    if (pid_prcs == -1)
        perror("process creation");
    else if (pid_prcs == 0)
    {
        char str[64];
        sprintf(str, "%d", runTime);
        if (execl("process.out", "process.out", str, NULL) == -1)
        {
            perror("Couldn't create the process");
            exit(10);
        }
    }
    else
    {
        return pid_prcs;
    }
};

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
};

void HPF()
{
    open_outputFile();
    priorityQueue *queue = new_PrioriyQueue();
    processData process;
    PCB currProcess, processToRun;
    int recValue = recvMessage(&process);
    bool noMoreProcesses = (recValue == 1);

    while (!priority_isEmpty(queue->head) || !noMoreProcesses) // As long as there's processes waiting or being processed
    {
        while (recValue == 0) // A process arrived
        {
            printf("\nHPF: Received process #%d at %d.\n", process.id, getClk());
            currProcess = create_PCB(&process);
            priority_enqueue(queue, &currProcess);
            recValue = recvMessage(&process);
            if (recValue == 1)
                noMoreProcesses = true;
        }
        if (priority_dequeue(queue, &processToRun))
        {
            // Creating process process
            int pid_process = create_process(processToRun.remainingTime);
            processToRun.pid = pid_process;
            processToRun.status = STARTED;
            processToRun.startTime = getClk();
            printThis(&processToRun);
            int STATUS;
            waitpid(pid_process, &STATUS, 0);
            processToRun.status = FINISHED;
            processToRun.TA = getClk() - processToRun.processStruct.arrivaltime;
            processToRun.remainingTime = 0;
            printThis(&processToRun);
        }
        recValue = recvMessage(&process);
    }
    printPerf();
    close_oputputFile();
    printf("\n************* EOS *************\n");
};

#endif