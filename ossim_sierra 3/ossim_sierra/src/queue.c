#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t * q) {
        if (q == NULL) return 1;
	return (q->size == 0);
}

void enqueue(struct queue_t * q, struct pcb_t * proc) {
        /* TODO: put a new process to queue [q] */
        if (q == NULL || proc == NULL) return;

        //check if queue full
        if (q -> size >= MAX_QUEUE_SIZE) {
                return;
        }
        //add process to the end of queue
        q -> proc[q -> size] = proc;
        q -> size++;
}

struct pcb_t * dequeue(struct queue_t * q) {
        /* TODO: return a pcb whose prioprity is the highest
         * in the queue [q] and remember to remove it from q
         * */
        if (q == NULL || q -> size == 0) return NULL;

        //find the process with priority

        int hipri_index = 0;
        for (int i = 1; i < q -> size; i++) {
                if (q -> proc[i] -> priority < q->proc[hipri_index] -> priority) {
                        hipri_index = i;
                }
        }
        
        //take highest priority
        struct pcb_t *proc = q -> proc[hipri_index];

        //other process +1
        for (unsigned long i = hipri_index; i < q -> size - 1; i++) {
                q -> proc[i] = q -> proc[i + 1];
        }

/*
        struct pcb_t * proc = q -> proc[0];
        for (int i = 0; i < q -> size - 1; i++){
                q -> proc[i] = q -> proc [i + 1];
        }
*/
        q -> size--;
        q -> proc[q -> size] = NULL;
	return proc;
}
