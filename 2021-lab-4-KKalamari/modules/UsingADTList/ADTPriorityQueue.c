///////////////////////////////////////////////////////////
//
// Υλοποίηση του ADT Priority Queue μέσω ταξινομημένης λίστας.
//
///////////////////////////////////////////////////////////

#include <stdlib.h>
#include <assert.h>

#include "ADTPriorityQueue.h"
#include "ADTList.h"			// Η υλοποίηση του PriorityQueue χρησιμοποιεί List


// Ενα PriorityQueue είναι pointer σε αυτό το struct
struct priority_queue {
	List list;		// η λίστα στην οποία αποθηκεύουμε τα στοιχεία
	ListNode Parent;
	DestroyFunc destroy_value;	// Συνάρτηση που καταστρέφει ένα στοιχείο 
	CompareFunc compare;		// Η διάταξη
	
};


// Συναρτήσεις του ADTPriorityQueue //////////////////////////////////////////////////

PriorityQueue pqueue_create(CompareFunc compare, DestroyFunc destroy_value, Vector values) {
	
	PriorityQueue Q=malloc(sizeof(*Q));
	
	Q->compare=compare;
	
	// Για την κλήση της συνάρτησης destroy_value, την περνάμε απλά στη list_create
	// οπότε θα καλείται αυτόματα όταν το στοιχείο αφαιρεθεί από τη λίστα.
	// ... = list_create(destroy_value);
	Q->list = list_create(destroy_value);		

	return NULL;
}

int pqueue_size(PriorityQueue pqueue) {
	return list_size(pqueue->list);
}

Pointer pqueue_max(PriorityQueue pqueue) {
	return NULL;
}

void pqueue_insert(PriorityQueue pqueue, Pointer value) {

}

void pqueue_remove_max(PriorityQueue pqueue) {

}

DestroyFunc pqueue_set_destroy_value(PriorityQueue pqueue, DestroyFunc destroy_value) {
	// Απλά καλούμε την αντίστοιχη συνάρτηση της λίστας
	return list_set_destroy_value(pqueue->list, destroy_value);
}

void pqueue_destroy(PriorityQueue pqueue) {
	list_destroy(pqueue->list);
}
