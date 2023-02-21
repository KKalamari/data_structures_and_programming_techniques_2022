#include <stdlib.h>
#include <stdio.h>

#include "ADTPriorityQueue.h"
#include "ADTSet.h"

int compare_ints(Pointer a, Pointer b) {
    return *(int*)a- *(int*)b;
}

int* create_int(int value) {
	int* pointer = malloc(sizeof(int));		// δέσμευση μνήμης
	*pointer = value;						// αντιγραφή του value στον νέο ακέραιο
	return pointer;
}


int main() {
    PriorityQueue pqueue= pqueue_create(compare_ints,free,NULL);  //δημιουργία της priority queue

    for(int i=0; i<20; i++) {
        pqueue_insert(pqueue,create_int(rand()));  // Εισαγωγή των τυχαίων στοιχείων 
    }

    while(pqueue_size(pqueue)>0) {
        printf("currrent max element: %d \n", *(int*)pqueue_max(pqueue)); // Εμφανίζει κάθε φορα το μέγιστο αριθμό του pqueue
        pqueue_remove_max(pqueue);
    }

    pqueue_destroy(pqueue);

    Set set=set_create(compare_ints,free);  

    for(int i=0; i<20; i++) // εισαγωγή των 20 ακεραίων στο σετ
        set_insert(set,create_int(rand()));

    for(SetNode Node=set_first(set);  // Εκτύπωση του σετ
        Node!=SET_EOF;
        Node=set_next(set, Node)) 
            printf("set element: %d\n", *(int*)set_node_value(set,Node));
    
    set_destroy(set);

}
