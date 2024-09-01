#ifndef PROCESS_DICT_H
#define PROCESS_DICT_H
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#include "priority_queue.h"

//node that holds the process id with its priority queue
struct listNode{
	int pid;
	struct p_queue *q;
	struct listNode *next;
	int write_state, read_state;
       	int32_t	value, priority; //temp values
};

//list of all the process specific priority queues
struct pq_list{
	int size;
	struct listNode *head;
};

typedef struct listNode listNode;
typedef struct pq_list pq_list;


pq_list* initList(void);
listNode* createlistEntry(pq_list*, int);
listNode* getListEntry(pq_list*, int);
void removeListEntry(pq_list*, int);

//initialize the list
pq_list* initList(void){
	pq_list *d = (pq_list*)kmalloc(sizeof(pq_list), GFP_ATOMIC);
	d->size = 0;
	d->head = NULL;

	return d;
}

//creates a listnode with process id
listNode* createlistEntry(pq_list *m, int pid){
	listNode *curr = (listNode*)kmalloc(sizeof(listNode), GFP_ATOMIC);
	curr->pid = pid;
	curr->q = NULL;
	curr->next = m->head;
	curr->write_state = 0;
	curr->read_state = 0;
	m->head = curr;
	m->size++;
	return curr;
}

//get the priority queue specific to the pid
listNode* getListEntry(pq_list *m, int pid){
	listNode *ptr = m->head;
	while(ptr != NULL){
		if(ptr->pid == pid){
			return ptr;
		}
		ptr = ptr->next;
	}
	return ptr;
}

//removes the entry from the list
void removeListEntry(pq_list *m, int pid){
	listNode *temp = m->head, *prev = NULL;
	if(temp && temp->pid == pid){
		m->head = temp->next;
		kfree(temp->q);
		kfree(temp);
		m->size--;
		return;
	}
	else{
		while(temp && temp->pid != pid){
			prev = temp;
			temp = temp->next;
		}
		if(!temp) return;
		prev->next = temp->next;
		kfree(temp->q);
		kfree(temp);
		m->size--;
	}
}

#endif


