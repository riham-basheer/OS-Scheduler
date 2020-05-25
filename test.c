#include "memTree.h"
#include<stdio.h>
#include<stdlib.h>
#include<time.h>


int createClock();

int main() {
	createClock();
	initClk();
	open_memory_log();
	memTree* mem = new_memTree();
	node* root;
	root = mem->root;
	Allocate_memory(root, 100, 1);
	Allocate_memory(root, 200, 2);
	Allocate_memory(root, 50 , 3);

	deallocate_process(root, 3);
	deallocate_process(root, 1);
	close_memlog_file();
	clean_mem(root);
	
	destroyClk(false);
	
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

