#include <stdlib.h>
#include <stdio.h>
#include "pair_sum.h"

//Διεσχίζω το vector μία φορά και για κάθε στοιχείο διεσχίζω 2η φορά το vector ξεκινόντας από το ακριβώς επόμενο
//στοιχείο και έλεγξε εάν το αθροισμα αυτών των στοιχείων είναι ίσο με το target
Pair pair_sum(int target, Vector numbers) {	
	for(int i=0; i<vector_size(numbers); i++) {  
		int* a= vector_get_at(numbers, i);   

		for(int x=i++; x<vector_size(numbers);x++) {
			int* b= vector_get_at(numbers, x);

			if(target== *(int*)a+ *(int*)b){
				Pair found=malloc(sizeof(found));
				found->first=*(int*)a;
				found->second=*(int*)b;
				return found;
			}
		}
		
	}
	return NULL;
}