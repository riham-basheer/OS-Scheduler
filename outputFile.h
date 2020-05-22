#ifndef OS_SCHEDULER_OUTPUTFILE_H
#define OS_SCHEDULER_OUTPUTFILE_H

#include "PCB.h"
#include <sys/ipc.h>
#include <math.h>
#include <stdio.h>

#define LOGFILE_NAME "scheduler.log"
#define PERFFILE_NAME "scheduler.perf"

FILE *logFile_ptr;
FILE *perfFile_ptr;
int counter = 0;
double totalWTA = 0, totalWT = 0;

void open_outputFile()
{
    logFile_ptr = fopen(LOGFILE_NAME, "w");
    perfFile_ptr = fopen(PERFFILE_NAME, "w");
};

void printThis(PCB *currPCB)
{
    int currTime = getClk();
    int id, arrTime, runTime, remTime, WT, TA,startTime,start_wait;
    id = currPCB->processStruct.id;
    arrTime = currPCB->processStruct.arrivaltime;
    runTime = currPCB->processStruct.runningtime;
    remTime = currPCB->remainingTime;
    startTime= currPCB->startTime;
    //WT = currPCB->startTime - arrTime; // TODO: to be edited for RR
    WT= currPCB->totalwaitTime;
    start_wait= currPCB->wait_at_start;


    switch (currPCB->status)
    {
    case STARTED:
        fprintf(logFile_ptr, "At time %d process %d started arr %d total runtime %d remain %d wait %d\n", startTime, id, arrTime, runTime, remTime, start_wait);
        break;
    case FINISHED:
        currTime= currPCB->finishTime;
        TA = currPCB->TA;
        totalWT = totalWT + WT;
        double WTA = (double)(TA / runTime);
        totalWTA = totalWTA + WTA;
        fprintf(logFile_ptr, "At time %d process %d finished arr %d total runtime %d remain %d wait %d TA %d WTA %0.2f\n",
                currTime, id, arrTime, runTime, remTime, WT, TA, WTA);
        counter = counter + 1;
        break;
    default:
        perror("outputFile: no such status");
        break;
    }
};

void printPerf()
{
    double avgWTA = (double)totalWTA / counter;
    fprintf(perfFile_ptr, "CPU Utilization = ");
    fprintf(perfFile_ptr, "\nAvg WTA = %0.2f", avgWTA);
    fprintf(perfFile_ptr, "\nAvg Waiting = %0.2f", (double)totalWT / counter);
    fprintf(perfFile_ptr, "\nStd WTA = ");
};

void close_oputputFile()
{
    fclose(logFile_ptr);
    fclose(perfFile_ptr);
};

#endif