#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include "priority_queue.h"
#include "process_dict.h"


struct proc_dir_entry *myproc_entry;
pq_list *processList;


static ssize_t myproc_write(struct file *fp, const char *buff, size_t len, loff_t *offset) {
    char in_buff[10];
    int8_t qsize;
    listNode *curr = getListEntry(processList, current->pid);

    if (curr == NULL) {
        printk(KERN_ALERT "No queue found for pid %d\n", current->pid);
        return -EACCES;
    }

    if (copy_from_user(in_buff, buff, len)) return -EFAULT;
    in_buff[len] = '\0';  

    //check the input from the user program and set the flag for Queue operation accordingly
    
    if (strcmp(in_buff, "push") == 0) {
        curr->write_state = 1; // Set to push state
        printk(KERN_INFO "Set write_state to 1 (push) for pid %d\n", current->pid);
        return len;
    } else if (strcmp(in_buff, "pop") == 0) {
        curr->read_state = 1; // Set to pop state
        printk(KERN_INFO "Set read_state to 1 (pop) for pid %d\n", current->pid);
        return len;
    } else if (strcmp(in_buff, "top") == 0) {
        curr->read_state = 2; // Set to read top state
        printk(KERN_INFO "Set read_state to 2 (top) for pid %d\n", current->pid);
        return len;
    }
	
	
    //set the size of the prioQueue
    	
    if (curr->q == NULL) {
        memcpy(&qsize, in_buff, sizeof(int8_t));

        if (qsize < 1 || qsize > 100) {
            printk(KERN_ALERT "Queue size must be between 1 and 100 inclusively for pid %d\n", current->pid);
            return -EINVAL;
        }
        printk(KERN_INFO "Initializing priority queue with max size of %d for pid %d\n", qsize, current->pid);
        curr->q = initQ(qsize);
        
    } else{
	//            printk(KERN_ALERT "We are here in else for pid %d and current write_state is %d\n", current->pid, curr->write_state);
        if (checkSize(curr->q) >= curr->q->capacity) {
            printk(KERN_ALERT "Queue is full for pid %d, cannot push more values\n", current->pid);
            return -ENOSPC;
        } else {
            if (curr->write_state == 1) {
                int32_t value;
                memcpy(&value, in_buff, sizeof(int32_t));
                printk(KERN_INFO "Read value %d from pid %d\n", value, current->pid);
                curr->value = value;
                curr->write_state = 2; // Ready for priority input
            } else if (curr->write_state == 2) {
                int32_t priority;
                memcpy(&priority, in_buff, sizeof(int32_t));
                printk(KERN_INFO "Read priority %d for value %d from pid %d\n", priority, curr->value, current->pid);
                curr->priority = priority;
                curr->write_state = 1; // Reset to accept new value
                push(curr->q, curr->value, curr->priority);
                printQueue(curr->q);
            }
        }
    }
    return len;
}


static ssize_t myproc_read(struct file *fp, char *buff, size_t len, loff_t *offset) {
    int32_t retval;
    char ret_buff[4];
    listNode *curr = getListEntry(processList, current->pid);

    if (curr == NULL || curr->q == NULL) {
        printk(KERN_ALERT "Queue is not initialized for pid %d\n", current->pid);
        return -EACCES;
    } 
    if (curr->q->size == 0) {
        printk(KERN_ALERT "Queue is empty for pid %d\n", current->pid);
        return -ENODATA;
    }

    if (curr->read_state == 2) {
        retval = top(curr->q);  // Just get the top value without popping it
        printk(KERN_INFO "Top value: %d for pid %d\n", retval, current->pid);
    } else {
        retval = pop(curr->q);  // Pop the value
        printk(KERN_INFO "Popped: %d for pid %d\n", retval, current->pid);
    }

    len = sizeof(int32_t);

    ret_buff[3] = (retval >> 24) & 0xFF;
    ret_buff[2] = (retval >> 16) & 0xFF;
    ret_buff[1] = (retval >> 8) & 0xFF;
    ret_buff[0] = retval & 0xFF;

    if (copy_to_user(buff, ret_buff, len)) return -EFAULT;
    return len;
}
static int myproc_open(struct inode *inode, struct file *file){
	listNode *curr = getListEntry(processList, current->pid);
	if(curr != NULL){
		printk(KERN_ALERT "multiple open for %d is not permitted\n", current->pid);
		return -EACCES;
	}
	try_module_get(THIS_MODULE);
	printk(KERN_ALERT "proc file is opened by %d\n", current->pid);
	createlistEntry(processList, current->pid);
	return 0;
}

static int myproc_close(struct inode *inode, struct file *file){
	module_put(THIS_MODULE);
	printk(KERN_ALERT "proc file is closed by pid %d", current->pid);
	removeListEntry(processList, current->pid);
	return 0;
}
		
static struct proc_ops myfops = {
	.proc_read = myproc_read,
	.proc_write = myproc_write,
	.proc_open = myproc_open,
	.proc_release = myproc_close
};

static int __init PQ_init(void){
	myproc_entry = proc_create("pq_basic", 0666, NULL, &myfops);
	if(myproc_entry == NULL){
		printk(KERN_ALERT "error : Not able to initialize proc entry\n");
	}else{
		printk(KERN_ALERT "proc enrty created successfully\n");
		processList = initList();
	}
	return 0;
}

static void __exit PQ_exit(void){
	//destroy list
	remove_proc_entry("pq_basic", NULL);
	printk(KERN_ALERT "pq_basic entry is removed\n");
}



module_init(PQ_init);
module_exit(PQ_exit);
MODULE_DESCRIPTION("LKM that implements priorityQueue algorithm per user process");
MODULE_AUTHOR("Sri Harsha");
MODULE_LICENSE("GPL");



