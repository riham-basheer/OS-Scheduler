#include "headers.h"
#include "messageBox.h"
#include "processQueue.h"

void clearResources(int);
bool readFromFile(processQueue *queue);
int createScheduler(char str1[64], char str2[64]);
int createClock();

int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    newMessageBox();
    // TODO Initialization
    // 1. Read the input files.
    processQueue *pQueue = new_processQueue();
    bool read = readFromFile(pQueue);
    if (!read)
        printf("\nError! Input file was not read.\n");

    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    int numAlgo, quantum;
    printf("\n*** Scheduling Algorithm ***");
    printf("\n[1] High Priority Fisrt (HPF).\n[2] Shortest Remaining Time Next (SRTN).\n[3] Round Robbin.");
    printf("\nType a number: ");
    scanf("%i", &numAlgo);
    if (numAlgo != 1 && numAlgo != 2 && numAlgo != 3)
    {
        printf("\nInvalid Algorithm. Please try again.\n");
        exit(5);
    }
    if (numAlgo == 3)
    {
        printf("Enter the quantum of RR: ");
        scanf("%i", &quantum);
    }

    // 3. Initiate and create the scheduler and clock processes.
    // We need to send the info the scheduler will need to begin its work.
    char numAlgo_string[64], quantum_string[64];
    sprintf(numAlgo_string, "%d", numAlgo);
    sprintf(quantum_string, "%d", quantum);

    //Asmaa: trying to 
    
    int pid_schd = createScheduler(numAlgo_string, quantum_string);
    int pid_clk = createClock();
    // 4. Use this function after creating the clock process to initialize clock
    initClk();
    /*Now, we'll send each process at its arrival time to the scheduler
    via a message box.*/
    processData *pData = dequeue(pQueue);
    int currTime = getClk();
    while (pData != NULL) // As long as there are processes in the processQueue
    {
        currTime = getClk();
        if (currTime == pData->arrivaltime)
        {
            if (!sendMessage(*pData))
            {
                perror("\nProcess was not sent to the schedule. \n");
                exit(9);
            }
            free(pData);
            pData = dequeue(pQueue);
        }
    }
    sendEOT(); // No more processes.

    // 7. Clear clock resources
    int STATUS;
    waitpid(pid_schd, &STATUS, 0);
    clear(pQueue);
    destroyClk(true); // End of simulation
    return 0;
}

/* Reads the processes from the file created by the test_generator.
Then, it creates a process structure for each one and add it to the 
process queue.*/
bool readFromFile(processQueue *queue)
{
    FILE *inputFile_ptr = fopen(PROCESSESFILE_NAME, "r");
    if (inputFile_ptr == NULL)
        return false;
    char buffer[265];
    while (fgets(buffer, sizeof(buffer), inputFile_ptr) != NULL)
    {
        if (buffer[0] != '#')
        {
            processData *pData = (processData *)malloc(sizeof(processData));
            // TODO: free this memory in termination.
            sscanf(buffer, "%d%d%d%d%d", &pData->id, &pData->arrivaltime, &pData->runningtime, &pData->priority, &pData->memory);
            enqueue(queue, pData);
        }
    }
    fclose(inputFile_ptr);
    return true;
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
    printf("\nClearing resources ... \n");
    deleteMessageBox();
};

int createScheduler(char str1[64], char str2[64])
{
    int pid_schd = fork();

    if (pid_schd == -1)
        perror("\nHi ! u have error in scedular fork ... \n");
    else if (pid_schd == 0)
    {
        //initiation :: //remaining time parameter
        if (execl("scheduler.out", "scheduler.out", str1, str2, NULL) == -1)
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

int createClock()
{
    int pid_clk = fork();
    if (pid_clk == -1)
        perror("HI ! I have error in clk fork ");

    else if (pid_clk == 0)
    {
        //initialization
        if (execl("clk.out", "clk.out", NULL) == -1)
        {
            printf("\nCouldn't create the clock..\n");
            exit(10);
        }
    }
    else
    {
        return pid_clk;
    }
}
