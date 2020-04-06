#define _POSIX_SOURCE
#include"headers.h"
#include "HPF.h"

int main(int argc, char *argv[])
{
	initClk();

	//picking an algorithm
	printf("\nEnterend Scheduler\n");
	int algoNum = (int)atoi(argv[1]);


	// round robin algorithm
	if (algoNum == 1)
	{
	//    HPF();
	}

	destroyClk(false);
}
