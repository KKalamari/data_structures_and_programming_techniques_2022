#include <stdlib.h>
#include <stdio.h>
#include "pair_sum.h"

int compare_ints(Pointer a, Pointer b) {
    return *(int*)a-*(int*)b;
}

//Εισάγω τα στοιχεία του vector στο map, κάνω διάσχιση του vector και για κάθε στοιχείο ελέγχω 
//εάν υπάρχει key για το οποίο ισχύει key== target-current_vector_value 
//εάν υπάρχει αποθηκεύω τα current_vector_value και b στο pair found και το κάνω return

Pair pair_sum(int target, Vector numbers) {
	
	Map map= map_create(compare_ints,free,free);

	for(int i=0; i<vector_size(numbers); i++)  //εισάγω τα στοιχεία του vector στο map
	map_insert(map,vector_get_at(numbers,i),vector_get_at(numbers,i));

	for(int x=0; x<vector_size(numbers); x++){
	int key = target -*(int*)vector_get_at(numbers,x);
	int* b=map_find(map,&key);

		if(b!=NULL) {
			Pair found=malloc(sizeof(found));
			found->first=*(int*)vector_get_at(numbers,x);
			found->second= *(int*) b;
			return found;
		}

	}

	return NULL;
}