
#ifndef MEMTREE
#define MEMTREE

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
//#include<stdbool.h>
#include "headers.h"



// log file 
#define LOGFILE_NAME "memory.log"
FILE *logFile_pt;
//

void open_memory_log(){
	logFile_pt = fopen(LOGFILE_NAME, "w");
	fprintf(logFile_pt,"# At time x allocated y bytes for process z from i to j\n");

}

void close_memlog_file(){
	 fclose(logFile_pt);
}


enum COLOR {
	WHITE, //free, ready to be poulated
	BLACK, //Occupied by another process
	RED    // an internal node, is further subdivided 
};
typedef struct node node;
struct node {
	node * left, * right;
	int process_id;
	int size;
	int start_end[2];
	enum COLOR color;

};

typedef struct memTree memTree;
struct memTree {
	node * root; 
};


// ------------------ //


memTree *new_memTree() {

	memTree *memtree = (memTree *)malloc(sizeof(memTree));
	 memtree->root = (node *)malloc(sizeof(node));

     //node *newroot
	memtree->root->left= NULL;
	memtree->root->right = NULL;
	memtree->root->process_id = -1;
	memtree->root->size = 1024;
	memtree->root->color = WHITE;
	memtree->root->start_end[0] = 0;
	memtree->root->start_end[1] = 1024;
	return memtree;
}

void sub_divide(node* root) {
	root->left= (node *)malloc(sizeof(node));
	root->right= (node *)malloc(sizeof(node));

	root->right->color = WHITE; 
	root->left->color = WHITE;

	root->right->process_id = -1;
	root->left->process_id = -1;

	root->right->size = (root->size) / 2;
	root->left->size = (root->size) / 2;

	root->right->left = NULL;
	root->right->right = NULL;

	root->left->left = NULL;
	root->left->right = NULL;

	root->left->start_end[0] = root->start_end[0];
	root->left->start_end[1] = root->left->start_end[0] + root->left->size;

	root->right->start_end[1] = root->start_end[1];
	root->right->start_end[0] = root->right->start_end[1] - root->left->size;

	
}

bool Allocate_memory(node* cur_node, int p_size, int p_id) {
	/*pass the root to this function
	returns false if allocation failed
	allocation fails for lack of a contiguous memory block available*/

	int allocated_size; 

	bool can_inhabit = (p_size <= cur_node->size && p_size > (cur_node->size) / 2);

	if (cur_node->color == WHITE) {
		switch (can_inhabit) {
		case 1:
			cur_node->color = BLACK;
			cur_node->process_id = p_id;

			//// for printing
			allocated_size = cur_node->size;
			int from = cur_node->start_end[0]; int to = cur_node->start_end[1];
			fprintf(logFile_pt, "At time %d allocated %d bytes for process %d from %d to %d\n",getClk(),allocated_size,p_id,from,to );
			///
			
			return true;
			break;

		case 0:
			cur_node->color = RED;
			sub_divide(cur_node);
			if (p_size > cur_node->size) { return false; }
			return Allocate_memory(cur_node->left, p_size, p_id);
			break;
		}


	}


	else if (cur_node->color == BLACK){
		return false;
	}


	else if (cur_node->color == RED) {

		bool done = Allocate_memory(cur_node->left, p_size, p_id);
		if (done) { return done; }
		if (!done) {
			return Allocate_memory(cur_node->right, p_size, p_id);
		}


	}




}

node* free_leaf(node* root, int p_id) {

	if (root == NULL) return NULL;
	root->left = free_leaf(root->left, p_id);
	root->right = free_leaf(root->right, p_id);
	
	if (root->process_id == p_id && root->left == NULL && root->right == NULL) {
		/////for printing
		int from = root->start_end[0]; int to = root->start_end[1];
		int freed_mem = root->size;
		fprintf(logFile_pt, "At time %d freed %d bytes from process %d from %d to %d\n",getClk(),freed_mem,p_id,from,to);

		
		free(root);
		root= NULL;
		
		return NULL;
	}

	return root;

}


int get_height(node* root) {
 
 if (root == NULL) return -1;

 int height_left = 0, height_right = NULL;
 node* p = root;

 while (p->left != NULL)
 {
  height_left++;
  p = p->left;

 }
 while (p->right != NULL) {
  height_right++;
  p = p->right;
 }
 if (height_left > height_right) { return height_left; }
 else { return height_right;  }
}
void re_merge(node* root) {
 if (root == NULL) return;

 bool leaf = root->left == NULL && root->right == NULL;

 if ( leaf && root->color != RED) return ;

 if (leaf && root->color == RED) { root->color = WHITE; return; }


 if (root->left == NULL && root->right->color == WHITE) {
  free(root->right);
  root->right = NULL;
  root->color = WHITE;
  return;
 }

 if (root->right == NULL && root->left->color == WHITE) {
  free(root->left);
  root->left = NULL;
  root->color = WHITE;
  return;
 }
 
 if ((root->right != NULL && root->left != NULL)) {
  bool both_white = (root->right->color == WHITE) && (root->left->color == WHITE);
  bool have_no_kids = (root->left->left == NULL && root->left->right == NULL) && (root->right->left == NULL && root->right->right == NULL);

  if (both_white && have_no_kids) {
   free(root->left);
   free(root->right);
   root->left = NULL;
   root->right = NULL;
   root->color = WHITE;

  }
 }
 re_merge(root->left);
 re_merge(root->right);
 
}
void re_merge_all(node* root) {
 if (root == NULL) { return; }
 if (root->left == NULL && root->right == NULL) { return; }
 int i;
 int h = get_height(root);
 if (h != 0) {
  for (i = 0; i < h+1; i++) {
   re_merge(root);
  }
 }
}

void deallocate_process(node* root, int p_id) {
 free_leaf( root, p_id);
 re_merge_all(root);
}


void clean_mem(node* root) {
	if (root == NULL) return;
	clean_mem(root->left);
	clean_mem(root->right);
	free(root);
}

#endif
