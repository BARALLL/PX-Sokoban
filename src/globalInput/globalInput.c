#include "globalInput.h"
#include <stddef.h>
#include <stdlib.h>

struct inputNode{
	int inputValue;
	struct inputNode *next;
};

static struct inputNode *inputBuffer = NULL;

static int cursorX, cursorY;


void setCursor(int x, int y) {
	/**
	 *set position of cursor
	 * */
	cursorX = x;
	cursorY = y;
}

void getCursor(int *x, int*y){
	/**
	 *get position of cursor
	 **/
	*x = cursorX;
	*y = cursorY;
}

void registerInput(int in){
		/**
		 *registers the input to the inputBuffer
		 *@param[in] input
		 * */
		struct inputNode *newNode = malloc(sizeof(struct inputNode));
		newNode->inputValue = in;
		newNode->next = NULL;	
		
		struct inputNode *node = inputBuffer;

		if(node == NULL) {
			inputBuffer = newNode;
			return;
		}

		while(node->next != NULL){
			node = node->next;
		}
		
		node->next = newNode;
}

int getInput(){
	/**
	 *returns oldest input in buffer
	 **/

	if(!inputBuffer) return -1;

	struct inputNode *buffer = inputBuffer;
	int toRet = inputBuffer->inputValue;
	inputBuffer = inputBuffer->next;
	free(buffer);
	

	return toRet;
	
}

void registerTerminal(){
	/**
	 *read input from terminal
	 **/
	int key = litClavier();
	if((key >= 0 && key < 4)
			|| key == 113
			|| key == 119
			|| key == 99
			|| key == 116
			|| key == 115
			|| key == 120
			|| key == 10
			|| key == 8) registerInput(key);
}

void flushInputBuffer(){
	while(inputBuffer != NULL){
		struct inputNode *tofree = inputBuffer;
		inputBuffer = inputBuffer->next;
		free(tofree);
	}
}
