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
double wta_array[150];
int i=-1; int total_run_times=0;

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
    WT= currPCB->totalwaitTime;
    start_wait= currPCB->wait_at_start;


    switch (currPCB->status)
    {
    case STARTED:
        fprintf(logFile_ptr, "At time %d process %d started arr %d total runtime %d remain %d wait %d\n", startTime, id, arrTime, runTime, remTime, start_wait);
        break;
    case FINISHED:
        i++;
        currTime= currPCB->finishTime;
        TA = currPCB->TA;
        totalWT = totalWT + WT;
        double WTA = (double)(TA / runTime);
        wta_array[i] = WTA;
        totalWTA = totalWTA + WTA;
        total_run_times += runTime;
        fprintf(logFile_ptr, "At time %d process %d finished arr %d total runtime %d remain %d wait %d TA %d WTA %0.2f\n",
                currTime, id, arrTime, runTime, remTime, WT, TA, WTA);
        counter = counter + 1;
        break;
    default:
        perror("outputFile: no such status");
        break;
    }
};


double find_std(){
    int numel= i+1, j;
    double STD, val, sum=0, mean= (double)totalWTA/numel;

    for (j=0; j<numel; j++){
       val= wta_array[j]-mean;
       sum += pow(val,2);
    }

    STD= sqrt((double)sum/numel);
    return STD;
}

void printPerf(int total_time)
{   
    double cpu_ut= ((double)total_run_times/ total_time)*100;
    double avgWTA = (double)totalWTA / counter;
    fprintf(perfFile_ptr, "CPU Utilization = % 0.2f %", cpu_ut);
    fprintf(perfFile_ptr, "\nAvg WTA = %0.2f", avgWTA);
    fprintf(perfFile_ptr, "\nAvg Waiting = %0.2f", (double)totalWT / counter);
    fprintf(perfFile_ptr, "\nStd WTA = %0.2f", find_std());
};

void close_oputputFile()
{
    fclose(logFile_ptr);
    fclose(perfFile_ptr);
};

#endif