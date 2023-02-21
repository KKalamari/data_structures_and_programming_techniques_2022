#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include "ADTQueue.h"
#include "ADTStack.h"


// Ένα Queue είναι pointer σε αυτό το struct.
struct queue {
    Stack stack,for_removal;
    Pointer front;
    int queue_steps;
};


Queue queue_create(DestroyFunc destroy_value) {
    Queue queue = malloc(sizeof(*queue));
    queue->stack = stack_create(destroy_value);
    queue->for_removal = stack_create(destroy_value);

    queue->queue_steps += 1;
	return queue;
}

int queue_size(Queue queue) {
    queue->queue_steps += 1;
	return stack_size(queue->stack)+stack_size(queue->for_removal);
}

Pointer queue_front(Queue queue) {
    queue->queue_steps += 1;
	return queue->front;
}

Pointer queue_back(Queue queue) {
    queue->queue_steps += 1;
    if(stack_size(queue->for_removal)!=0) {
        return stack_top(queue->for_removal);
    }
	return stack_top(queue->stack);
}

void queue_insert_back(Queue queue, Pointer value) {  
    stack_insert_top(queue->stack,value);

    if(stack_size(queue->stack)==1) 
        queue->front = stack_top(queue->stack);
    
    queue->queue_steps += 1;
}

void queue_remove_front(Queue queue) {
    if(stack_size(queue->stack)==0 && stack_size(queue->for_removal)==0)
        return;

    if(stack_size(queue->for_removal)==0) {
        while(stack_size(queue->stack)!=0) {
            stack_insert_top(queue->for_removal,stack_top(queue->stack));
            stack_remove_top(queue->stack);
        }
        stack_remove_top(queue->for_removal);
        queue->front = stack_top(queue->for_removal);
        queue->queue_steps += stack_size(queue->stack);
    }
    else{
        stack_remove_top(queue->for_removal);
       
        queue->queue_steps += 1;

        if(stack_size(queue->for_removal)!=0)
            queue->front = stack_top(queue->for_removal);
        else
            return;
    }

}

DestroyFunc queue_set_destroy_value(Queue queue, DestroyFunc destroy_value) {
    queue->queue_steps += 1;
	return stack_set_destroy_value(queue->stack,destroy_value);
}

int queue_steps(Queue queue) {
    return queue->queue_steps;
}

void queue_destroy(Queue queue) {
    stack_destroy(queue->stack);
    stack_destroy(queue->for_removal);
    free(queue);
}