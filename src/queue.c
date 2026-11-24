#include <stdio.h>
#include <stdlib.h>
#include "queue.h"

int empty(struct queue_t *q)
{
        if (q == NULL)
                return 1;
        return (q->size == 0);
}

void enqueue(struct queue_t *q, struct pcb_t *proc)
{
        /* TODO: put a new process to queue [q] */
        if(q == NULL || proc == NULL)
                return;

        // Skip if the queue is full
        if(q->size >= MAX_QUEUE_SIZE)
                return;

        q->proc[q->size] = proc;
        q->size++;
}

struct pcb_t *dequeue(struct queue_t *q)
{
        /* TODO: return a pcb whose prioprity is the highest
         * in the queue [q] and remember to remove it from q
         * */

	if(q == NULL || q->size == 0){
                return NULL;
        }

        struct pcb_t* proc = q->proc[0];
        for(int i = 1; i < q->size; i++){
                if(q->proc[i]->priority < proc->priority){
                        proc = q->proc[i];
                }
        }

        return purgequeue(q, proc);
}

struct pcb_t *purgequeue(struct queue_t *q, struct pcb_t *proc)
{
        /* TODO: remove a specific item from queue
         * */
        if(q == NULL || proc == NULL)
                return NULL;

        int index;
        for(index = 0; index < q->size; index++){
                if(q->proc[index] == proc){
                        break;
                }
        }

        if(index == q->size)
                return NULL;

        struct pcb_t* return_proc = q->proc[index];
        while(index < q->size - 1){
                q->proc[index] = q->proc[index+1];
                index++; 
        }
        q->size--;
        return return_proc;
}