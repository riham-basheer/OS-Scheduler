
#ifndef OS_SCHEDULER_HPF_H
#define OS_SCHEDULER_HPF_H

#include <unistd.h>
#include "priorityQueue.h"
#include "messageBox.h"
#include "outputFile.h"
#include "PCB.h"
#include "memTree.h"

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
    pcb.processStruct.memory = process->memory;
    pcb.processStruct.priority = process->priority;
    pcb.processStruct.runningtime = process->runningtime;
    pcb.remainingTime = process->runningtime;
    pcb.status = STOPPED;
    return pcb;
};

void HPF()
{   
    //create memory tree and open memory logfile
    open_memory_log();
	memTree* mem = new_memTree();
	node* root;
	root = mem->root;

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
            //allocate process in memory
            Allocate_memory(root, processToRun.processStruct.memory ,processToRun.processStruct.id);

            // Creating process process
            int pid_process = create_process(processToRun.remainingTime);
            processToRun.pid = pid_process;
            processToRun.status = STARTED;
            processToRun.startTime = getClk();
            processToRun.wait_at_start= processToRun.startTime-processToRun.processStruct.arrivaltime;
            processToRun.totalwaitTime= processToRun.wait_at_start;
            printThis(&processToRun);
            int STATUS;
            waitpid(pid_process, &STATUS, 0);
            processToRun.status = FINISHED;
            processToRun.finishTime= getClk();
            processToRun.TA = getClk() - processToRun.processStruct.arrivaltime;
            processToRun.remainingTime = 0;
            printThis(&processToRun);
            deallocate_process(root, processToRun.processStruct.id);
        }
        recValue = recvMessage(&process);
    }
    printPerf();
    close_oputputFile();
    close_memlog_file();
	clean_mem(root); // clean up the memory tree
    printf("\n************* EOS *************\n");
};

#endif