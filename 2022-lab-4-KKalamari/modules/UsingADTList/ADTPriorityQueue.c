///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Priority Queue μέσω ταξινομημένης λίστας.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>
#include "stdio.h"
#include "ADTPriorityQueue.h"
#include "ADTList.h"			// Η υλοποίηση του PriorityQueue χρησιμοποιεί List


// Ενα PriorityQueue είναι pointer σε αυτό το struct
struct priority_queue {
	List list;		// η λίστα στην οποία αποθηκεύουμε τα στοιχεία
	CompareFunc compare;
	DestroyFunc destroy_value;
};



// Βοηθητικές συναρτήσεις //////////////////////////////////////////////////////////////////
// Αρχικοποιεί την ταξινομημένη λίστα από τα στοιχεία του vector values.

static void naive_initialize_list(PriorityQueue pqueue, Vector values) {
	// Απλά κάνουμε insert τα στοιχεία ένα ένα (όπως η naive_heapify).
	// Προαιρετικά: sort τα στοιχεία και εισαγωγή όλα μαζί (πιο αποδοτικό).
	int size = vector_size(values);
	for (int i = 0; i < size; i++)
		pqueue_insert(pqueue, vector_get_at(values, i));
}


// Συναρτήσεις του ADTPriorityQueue ////////////////////////////////////////////////////////

PriorityQueue pqueue_create(CompareFunc compare, DestroyFunc destroy_value, Vector values) {
	PriorityQueue pqueue = malloc(sizeof(*pqueue));

	// Για την κλήση της συνάρτησης destroy_value, την περνάμε απλά στη list_create
	// οπότε θα καλείται αυτόματα όταν το στοιχείο αφαιρεθεί από τη λίστα.
	pqueue->list = list_create(destroy_value);
	pqueue->compare = compare;

	// Αν values != NULL, αρχικοποιούμε την ταξινομημένη λίστα.
	if (values != NULL)
		naive_initialize_list(pqueue, values);

	return pqueue;
}

int pqueue_size(PriorityQueue pqueue) {
	return list_size(pqueue->list);
}

Pointer pqueue_max(PriorityQueue pqueue) {
	if(list_size(pqueue->list)==0)
		return NULL;
	Pointer pq_max = list_node_value(pqueue->list,list_first(pqueue->list));
	for(ListNode max = list_first(pqueue->list);
		max != LIST_EOF;
		max = list_next(pqueue->list,max)) {

		if(pqueue->compare(pq_max,list_node_value(pqueue->list,max))<0)
			pq_max = list_node_value(pqueue->list,max);
		

	}
	printf("%d \n",*(int*)pq_max);
	return pq_max;
}

void pqueue_insert(PriorityQueue pqueue, Pointer value) {
	list_insert_next(pqueue->list,list_last(pqueue->list),value);
}

void pqueue_remove_max(PriorityQueue pqueue) {
	ListNode prev_node;
	ListNode prev_prev_node;
	if(list_size(pqueue->list)==0)
		return;
	Pointer max = pqueue_max(pqueue);
	if(list_node_value(pqueue->list,list_first(pqueue->list))==max) {
		list_remove_next(pqueue->list,NULL);
		return;
	}
	for( prev_node = list_first(pqueue->list);     
   		prev_node != list_last(pqueue->list);                          
   		prev_node = list_next(pqueue->list, prev_node)) {            
		prev_prev_node=prev_node;
		if(pqueue->compare(list_next(pqueue->list,prev_node),max)==0)
			list_remove_next(pqueue->list,prev_node);
	}

	list_remove_next(pqueue->list,prev_prev_node);
	return;
	
}

DestroyFunc pqueue_set_destroy_value(PriorityQueue pqueue, DestroyFunc destroy_value) {
	// Απλά καλούμε την αντίστοιχη συνάρτηση της λίστας
	return list_set_destroy_value(pqueue->list, destroy_value);
}

void pqueue_destroy(PriorityQueue pqueue) {

}
