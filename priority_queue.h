#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/uaccess.h>
#define EOFLOW 1
#define EUFLOW 2

//priority queue node -->each node has a value and its priority
struct pq_node{
	int data;
	int prio;
	struct pq_node *next;
};

//priority queue of the current process
struct p_queue{
	int size;
	int capacity;
	struct pq_node *head;
};

typedef struct pq_node pq_node;
typedef struct p_queue p_queue;
p_queue* initQ(int qsize);
int checkSize(p_queue* q);
void push(p_queue* q, int data, int prio);
int top(p_queue* q);
int pop(p_queue* q);
void printQueue(p_queue* q);

//initializes the queue and returns the head of the queue
p_queue* initQ(int qsize){
	p_queue *q = (p_queue*)kmalloc(sizeof(p_queue)*1, GFP_ATOMIC);
	q->size = 0;
	q->capacity = qsize;
	q->head = NULL;
	return q;
}

//returns the current size of the queue

int checkSize(p_queue *q){
/*	if(q->size > q->capacity) return -EOFLOW;
	if(q->size <= 0) return -EUFLOW;
	return 0;*/
	return q->size;
}

//push a new element into the priority queue
void push(p_queue *q, int data, int prio){
	
	pq_node *temp = (pq_node*)kmalloc(sizeof(pq_node), GFP_ATOMIC);
	temp->data = data;
	temp->prio = prio;
	temp->next = NULL;
	if(q->head == NULL){
		q->head = temp;
		q->size += 1;
		return ;
	}
	if(q->head->prio > temp->prio){
		temp->next = q->head;
		q->head = temp;
		q->size += 1;
		return ;
	}
	pq_node *curr = q->head;
	while(curr->next != NULL && curr->next->prio <= temp->prio){
		curr = curr->next;
	}
	curr->next = temp;
	q->size += 1;
	return ;
}

//return the top of the priority queue
int top(p_queue *q){
	return q->head->data;
}

//remove the minimum element node from the queue
int pop(p_queue *q){
	int ret;
	pq_node *temp;
	temp = q->head;
	q->size -= 1;
	q->head = q->head->next;
	ret = temp->data;
	kfree(temp);
	return ret;
}


//print the priority queue
void printQueue(p_queue *q){
	if(q == NULL || q->size <= 0){
		printk(KERN_INFO "No data to print\n");
		return ;
	}
	struct pq_node *curr = q->head;

	printk(KERN_INFO "-------queue had %d elements-------\n", q->size);
	while(curr){
		printk(KERN_INFO "-------%d | %d\n", curr->data, curr->prio);
		curr = curr->next;
	}
	printk(KERN_INFO "-----------------------------------\n");

	return;
}
#endif // PRIORITY_QUEUE_H

