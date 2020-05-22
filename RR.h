#ifndef OS_SCHEDULER_RR_H
#define OS_SCHEDULER_RR_H

#include "HPF.h"
#include <stdio.h>
#include <math.h>
#include "PCB.h"
#include "processData.h"
#include "processQueue.c"
#include "PCBqueue.h"
#include "messageBox.h"
#include <signal.h>
#include "outputFile.h"



int get_process_situation(PCB process, int quantum){
	bool first= (process.nquanta_taken == 0);
	bool done = (process.remainingTime <= quantum);

	if (first && done){return 1;}
	if (!first && done) return 2;
	if (first && !done) return 3;
	if (!first && !done) return 4;

}

void type_1_process(PCB* processToRun){

		int pid_process = create_process(processToRun->remainingTime);
		processToRun->pid= pid_process;
		processToRun->status = STARTED;
		printf("At time %d process number %d started, arrivaltime= %d, totalruntime= %d remaining= %d wait= %d\n",
				getClk(), processToRun->processStruct.id, processToRun->processStruct.arrivaltime,
				processToRun->processStruct.runningtime, processToRun->remainingTime,
				getClk() - processToRun->processStruct.arrivaltime);

		processToRun->startTime = getClk();

		//////print
		//printThis(&processToRun);

		int STATUS;
		waitpid(pid_process, &STATUS, 0);
		processToRun->TA = getClk() - processToRun->startTime;
		processToRun->remainingTime = 0;
		processToRun->status = FINISHED;
		printf("At time %d process %d finished arr %d total %d remain %d wait %d finished at %d\n",
				getClk(), processToRun->processStruct.id, processToRun->processStruct.arrivaltime,
				processToRun->processStruct.runningtime, processToRun->remainingTime,
				getClk() - processToRun->processStruct.arrivaltime, processToRun->TA + processToRun->startTime);
		//printThis(&processToRun);
}

void type_2_process(PCB* processToRun){


	int pid_process= processToRun->pid;
	
	kill(pid_process,SIGCONT); //coninue preempted signal

	int STATUS;
		waitpid(pid_process, &STATUS,0);
		printf("\nexit status = %d\n ", STATUS);
		processToRun->TA = getClk() - processToRun->startTime;
		processToRun->remainingTime = 0;
		processToRun->status = FINISHED;
	printf("At time %d process %d finished, arrvaltime= %d runtime=%d remain %d waittime= %d finished at %d\n",
				getClk(), processToRun->processStruct.id, processToRun->processStruct.arrivaltime,
				processToRun->processStruct.runningtime, processToRun->remainingTime,
				getClk() - processToRun->processStruct.runningtime, processToRun->TA + processToRun->startTime);
	//printThis(&processToRun);

}

int give_quantum_first_time(int quantum, int remaining_time){
	
	int pid_schd = fork();
	if (pid_schd == -1) {perror("\nmamamamamam\n");}
	
	else if (pid_schd == 0)
	{
		char str[64];
		sprintf(str, "%d", remaining_time);
		if (execl("process.out", "process.out", str, NULL) == -1)
		{
			perror("\nCouldn't create the scheduler.\n");
			exit(10);
		}
	}
	
	sleep(quantum);
	kill(pid_schd, SIGSTOP);

	return pid_schd;

}

void give_quantum(int pid,int quantum){
	
	
	kill(pid, SIGCONT);
	sleep(quantum);
	kill(pid, SIGSTOP);

	


}



void RR ( int quantum) {
	open_outputFile();
	//deb
	//PCB* P= (PCB*)malloc(sizeof(PCB));
	//processQueue *queue= new_processQueue(); // ready queue

	PCBqueue *queue= new_PCBqueue(); // ready queue
	queue->head = NULL;
	connectToMessageBox();
	processData process;
	
	PCB currProcess, processToRun;
	int recValue = recvMessage(&process);
	bool noMoreProcesses = (recValue == 1);
	int process_situation;
	
	

	while (!PCB_isEmpty(queue->head) || !noMoreProcesses)
	{	
		while (recValue == 0)
		{
			printf("\nReceived a process now. It's # %d\n", process.id);
			currProcess = create_PCB(&process);
			
			currProcess.nquanta_taken =0;
			currProcess.remainingTime = currProcess.processStruct.runningtime; 
			
			PCB_enqueue(queue, &currProcess);
			recValue = recvMessage(&process);
			
			if (recValue == 1)
            noMoreProcesses = true;
		}
   
	
		while (!PCB_isEmpty(queue->head))
		{	
			
			PCB_dequeue(queue, &processToRun);
			
			printf("\ndequeue id= %d\n",processToRun.processStruct.id);
			int process_situation= get_process_situation(processToRun, quantum);
			
			//deb
			printf("\nprocess # %d in sitution= %d\n",processToRun.processStruct.id,process_situation);
			//deb
			
			if (process_situation==1){ type_1_process(&processToRun);}

			else if (process_situation==2){type_2_process(&processToRun);}

			else if (process_situation==3){
				
				processToRun.status = STARTED;
					processToRun.startTime = getClk();
				printf("At time %d process number %d started, arrivaltime= %d, totalruntime= %d remaining= %d wait= %d\n",
				getClk(), processToRun.processStruct.id, processToRun.processStruct.arrivaltime,
				processToRun.processStruct.runningtime, processToRun.remainingTime,
				getClk() - processToRun.processStruct.arrivaltime);

				//printThis(&processToRun);

				int pid_process = give_quantum_first_time(quantum, processToRun.remainingTime);
				processToRun.pid= pid_process;
				processToRun.status = STOPPED;
				processToRun.remainingTime =processToRun.remainingTime- quantum;
				processToRun.nquanta_taken++ ;
				PCB_enqueue(queue, &processToRun); 
				//printf("enqueue success?? = %d",enq);
			
			
			}


			else if (process_situation==4){
				processToRun.status = RESUMED;
				give_quantum(processToRun.pid, quantum);

				processToRun.remainingTime =processToRun.remainingTime- quantum;
				processToRun.nquanta_taken++ ;
				processToRun.status = STOPPED;
				PCB_enqueue(queue, &processToRun); 
			}

		
			
		}
		
		
		recValue= recvMessage(&process);
		
		
		
	}
	printPerf();
    close_oputputFile();
    printf("\n************* EOS *************\n");

}



#endif
