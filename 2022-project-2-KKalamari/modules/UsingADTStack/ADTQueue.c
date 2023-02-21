///////////////////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Queue μέσω του ADT Stack (dependent data structure)
//
///////////////////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTQueue.h"
#include "ADTStack.h"

// Ένα Queue είναι pointer σε αυτό το struct.
struct queue {
	Stack stack;
	Pointer front;
	int queue_steps;
};


Queue queue_create(DestroyFunc destroy_value) {
	Queue queue = malloc(sizeof(*queue));
	queue->stack = stack_create(destroy_value);
	queue->queue_steps += 1;

	return queue;
}

int queue_size(Queue queue) {
	queue->queue_steps += 1;
	return stack_size(queue->stack);
}

Pointer queue_front(Queue queue) {
	queue->queue_steps += 1;
	return queue->front;
}

Pointer queue_back(Queue queue) {
	queue->queue_steps += 1;
	return stack_top(queue->stack);
}

void queue_insert_back(Queue queue, Pointer value) {	
	stack_insert_top(queue->stack,value);
	
	if(stack_size(queue->stack)==1)
		queue->front=stack_top(queue->stack);
	
	queue->queue_steps += 1;		

}

void queue_remove_front(Queue queue) {	
	if(stack_size(queue->stack)==0)
		return;
	Stack decoy = stack_create(NULL);
	int size = stack_size(queue->stack);
	for(int i = 0; i<size; i++) {
		stack_insert_top(decoy,stack_top(queue->stack));
		stack_remove_top(queue->stack);
	}
	stack_remove_top(decoy);
	if(stack_size(decoy)==0) {
		stack_destroy(decoy);
		return;
	}
	queue->front = stack_top(decoy);
	while(stack_size(decoy)!=0) {
		stack_insert_top(queue->stack,stack_top(decoy));
		stack_remove_top(decoy);
	}
	stack_destroy(decoy);
	queue->queue_steps += 2*size-1;  //Βγάζουμε και βάζουμε τα στοιχεία στο queue->stack με εξαίρεση το front που αφαιρέθηκε

}

int queue_steps(Queue queue) {
	return queue->queue_steps;
}

DestroyFunc queue_set_destroy_value(Queue queue, DestroyFunc destroy_value) {
	queue->queue_steps += 1;

	return stack_set_destroy_value(queue->stack,destroy_value);
}

void queue_destroy(Queue queue) {
	stack_destroy(queue->stack);
	free(queue);
}