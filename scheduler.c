#define _POSIX_SOURCE
#include "headers.h"
#include "HPF.h"
#include "messageBox.h"

int main(int argc, char *argv[])
{
	// Initiate connection with clock and messagebox
	initClk();
	connectToMessageBox();

	// Choosing Algorithm
	int algoNum = (int)atoi(argv[1]);

	switch (algoNum)
	{
	case 1:
		HPF();
		break;
	case 2:
		break;
	case 3:
		break;
	default:
		printf("\nIncorrect Algorithm Number! Please try again.\n");
	}

	// End connection with clock
	destroyClk(false);
}
