#define _POSIX_SOURCE
#include "headers.h"
#include "HPF.h"
#include "RR.h"
#include "messageBox.h"

int main(int argc, char *argv[])
{
	// Initiate connection with clock and messagebox
	initClk();
	connectToMessageBox();

	// Choosing Algorithm
	int algoNum = (int)atoi(argv[1]);
	int quantum;

	switch (algoNum)
	{
	case 1:
		HPF();
		break;
	case 2:
		break;
	case 3:
		quantum= (int) atoi(argv[2]);
		RR(quantum);
		break;
	default:
		printf("\nIncorrect Algorithm Number! Please try again.\n");
	}

	// End connection with clock
	destroyClk(false);
}
