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
#include "memTree.h"



int get_process_situation(PCB process, int quantum){
	/* categorize process into types according to their remaining time and whether or not this 
	is their first time entering the processor (first quantum)*/

	bool first= (process.nquanta_taken == 0);
	bool done = (process.remainingTime <= quantum);

	if (first && done){return 1;} // first quantum & remainingtime < quantum
	if (!first && done) return 2; // not first & remainingtime < quantum
	if (first && !done) return 3; // first quantum & remainingtime > quantum
	if (!first && !done) return 4; // not first & remainingtime < quantum

}

void type_1_process(PCB* processToRun, node* root){
		/* handling a process whose entire runtime =< quantum, 
		just fork, exec and wait for it to finish 
		*/
		
		
		//execute process
		int pid_process = create_process(processToRun->remainingTime);

		// store start info
		processToRun->startTime = getClk();
		processToRun->wait_at_start= processToRun->startTime-processToRun->processStruct.arrivaltime;
		processToRun->pid= pid_process; //set pid in Process PCB
		processToRun->status = STARTED; //set status

		//print start info
		printf("At time %d process number %d started, arrivaltime= %d, totalruntime= %d wait= %d\n",
				processToRun->startTime, processToRun->processStruct.id, processToRun->processStruct.arrivaltime,
				processToRun->processStruct.runningtime, 
				processToRun->wait_at_start);

		printThis(&(*processToRun));

		//waiting for it to finish
		int STATUS;
		waitpid(pid_process, &STATUS, 0);
		

		//store finish info
		processToRun->finishTime = getClk();
		processToRun->status = FINISHED;
		processToRun->TA = processToRun->finishTime - processToRun->processStruct.arrivaltime;
		processToRun->remainingTime = 0;
		processToRun->totalwaitTime= processToRun->TA - processToRun->processStruct.runningtime;
		
		//print out finish info
		printf("At time %d process %d finished arr %d total runtime %d totalwait %d \n",
				processToRun->finishTime, processToRun->processStruct.id, processToRun->processStruct.arrivaltime,
				processToRun->processStruct.runningtime,
				processToRun->totalwaitTime);

		printThis(&(*processToRun));
		//free memory
		deallocate_process(root,processToRun->processStruct.id);
}

void type_2_process(PCB* processToRun,node* root){
	/* handle a whose remaining time <= quantum, and is currently paused */

	//coninue preempted process
	int pid_process= processToRun->pid;
	processToRun->status= RESUMED;
	kill(pid_process,SIGCONT); 

	//wait for it to finish
	int STATUS;
	waitpid(pid_process, &STATUS,0);
	
	//store finish info
	processToRun->finishTime = getClk();
	processToRun->status = FINISHED;
	processToRun->TA = processToRun->finishTime - processToRun->processStruct.arrivaltime;
	processToRun->remainingTime = 0;
	processToRun->totalwaitTime= processToRun->TA - processToRun->processStruct.runningtime;
	
	//print out finish info
	printf("\nexit status = %d\n ", STATUS);
	printf("At time %d process %d finished arr %d total runtime %d totalwait %d \n",
			processToRun->finishTime, processToRun->processStruct.id, processToRun->processStruct.arrivaltime,
			processToRun->processStruct.runningtime,
			processToRun->totalwaitTime);

	printThis(&(*processToRun));

	//free memory
	deallocate_process(root,processToRun->processStruct.id);
}

int give_quantum_first_time(int quantum, int remaining_time){
	/* fork and exec (create) a process and give it 1 quantum to execute */

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
	/*continue pre-empted process give it a quantum, then stop it*/
	
	kill(pid, SIGCONT);
	sleep(quantum);
	kill(pid, SIGSTOP);

}



void RR ( int quantum) {

	//setting up memory tree and opening memory log file
	//create memory tree and open memory logfile
    open_memory_log();
	memTree* mem = new_memTree();
	node* root;
	root = mem->root;

	//setting up message connection and scheduler output files
	open_outputFile();
	connectToMessageBox();

	//variables
	processData process;
	bool allocation_success;
	PCBqueue *queue= new_PCBqueue(); // ready queue
	PCB currProcess, processToRun;
	int process_situation;

	//listen for the first message (first process to handle)
	int recValue = recvMessage(&process);
	bool noMoreProcesses = (recValue == 1);
	
	
	
	while (!PCB_isEmpty(queue->head) || !noMoreProcesses)
	{	
		//following while: to listen for any more incoming processes and enqueue them
		while (recValue == 0)
		{ 
			printf("\nReceived a process now. It's # %d\n time now= %d", process.id, getClk());
			currProcess = create_PCB(&process);
			
			currProcess.nquanta_taken =0;
			currProcess.remainingTime = currProcess.processStruct.runningtime; 
			
			PCB_enqueue(queue, &currProcess);
			recValue = recvMessage(&process);
			
			if (recValue == 1)
            noMoreProcesses = true;
		}
   

		//following while: to enqueue processes from the ready queue and handle them
		if (!PCB_isEmpty(queue->head))
		{	
			
			PCB_dequeue(queue, &processToRun);
			
			printf("\ndequeue id= %d\n",processToRun.processStruct.id);
			int process_situation= get_process_situation(processToRun, quantum);
			
			//deb
			printf("\nprocess # %d in sitution= %d, it's remaining time = %d it has taken %d quanta so far\n",processToRun.processStruct.id,process_situation, processToRun.remainingTime, processToRun.nquanta_taken);
			//deb
			
			if (process_situation==1){ 
				//allocate memory
				allocation_success= Allocate_memory(root, processToRun.processStruct.memory, processToRun.processStruct.id);
				if(allocation_success)
				{
						type_1_process(&processToRun,root);
				}
				else{
					printf("\n there're not enough memory for process # %d now so I'll wait till %d bytes of memory is freed\n", processToRun.processStruct.id, processToRun.processStruct.memory);
					PCB_enqueue(queue, &processToRun); // re-enque without executing
				}
			}

			else if (process_situation==2){type_2_process(&processToRun,root);}

			else if (process_situation==3){
				//allocate memory
				allocation_success= Allocate_memory(root, processToRun.processStruct.memory,processToRun.processStruct.id);

				if (allocation_success){
				//store start data
				processToRun.startTime = getClk();
				processToRun.wait_at_start= processToRun.startTime-processToRun.processStruct.arrivaltime;
				processToRun.status = STARTED; //set status
				
				// give quantum
				int pid_process = give_quantum_first_time(quantum, processToRun.remainingTime);
				
				//print start info
				printf("At time %d process number %d started, arrivaltime= %d, totalruntime= %d wait= %d\n",
				processToRun.startTime, processToRun.processStruct.id, processToRun.processStruct.arrivaltime,
				processToRun.processStruct.runningtime, 
				processToRun.wait_at_start);

				printThis(&processToRun);
				
				//store pause info
				processToRun.pid= pid_process;
				processToRun.status = STOPPED;
				processToRun.remainingTime =processToRun.remainingTime- quantum;
				processToRun.nquanta_taken++ ;

				//re-enqueue
				PCB_enqueue(queue, &processToRun); }
				
				else{
					printf("\n there're not enough memory for process # %d now so I'll wait till %d bytes of memory is freed\n", processToRun.processStruct.id, processToRun.processStruct.memory);
					PCB_enqueue(queue, &processToRun); // re-enque without executing
				}

			
			
			}


			else if (process_situation==4){
				processToRun.status = RESUMED;
				give_quantum(processToRun.pid, quantum);

				processToRun.remainingTime =processToRun.remainingTime- quantum;
				processToRun.nquanta_taken++ ;
				processToRun.status = STOPPED;
				PCB_enqueue(queue, &processToRun); 
			}

     		/*
			recValue= recvMessage(&process);
			if (recValue==0){
				printf("\nReceived a process now. It's # %d\n", process.id);
			currProcess = create_PCB(&process);
			
			currProcess.nquanta_taken =0;
			currProcess.remainingTime = currProcess.processStruct.runningtime; 
			
			PCB_enqueue(queue, &currProcess);
			
			}*/
			
		}
		
		
		recValue= recvMessage(&process);
	
		
		
	}
	printPerf();
    close_oputputFile();
	close_memlog_file();
	clean_mem(root); // clean up the memory tree
    printf("\n************* EOS *************\n");
	

}



#endif
