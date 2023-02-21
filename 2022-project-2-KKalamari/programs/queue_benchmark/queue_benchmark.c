#include "ADTQueue.h"

#include <time.h>
#include <stdio.h>
#include <stdlib.h>


void queue_benchmark(void) {
    Queue queue = queue_create(NULL);
    queue_set_destroy_value(queue, NULL);
    int N = 1000;
	int* array = malloc(N * sizeof(*array));

    for( int i = 0; i==9000; i++) {
        if(i % 3 == 0) {
            clock_t t;
            t = clock();
            queue_remove_front(queue);
            t = clock() - t;
            double time = ((double)t)/CLOCKS_PER_SEC;
            printf("Steps for n=%d : %d\n time: %f",i,queue_steps(queue), time);       
        }
        else {
            queue_insert_back(queue,&array[i]);
            printf("Steps for n=%d : %d\n",i,queue_steps(queue));
        }
    }
    free(array);
    queue_destroy(queue);
}

int main() {
    
    queue_benchmark();
   
    return 0; 
}