#include <linux/init.h>
#include <linux/semaphore.h>
#include<linux/module.h>
#include<linux/kernel.h>
#include<linux/kthread.h>
#include<linux/sched.h>
#include<linux/time.h>
#include<linux/timer.h>
#include<linux/delay.h>


struct semaphore rw_mutex;
struct semaphore mutex;

static int read_count =0;
static int rw=0;
static int shared_var = 0;
static int count =0;

static int parm_writer = 1;
static int parm_reader = 5;

static struct task_struct *thread1;
static struct task_struct *thread2[5];

int write(void *no){

	int rw = (int) no;

	printk(KERN_INFO "%d Writer is waiting\n",rw); 
	down(&rw_mutex);
	printk(KERN_INFO "%d is writring\n", rw);
	shared_var++;
	count++;

	up(&rw_mutex);
	do_exit(0);
	return 0;
}

int read(void *no){


	int rw = (int) no;


	printk(KERN_INFO "%d reader is waiting\n",rw);
	down(&mutex);
	printk(KERN_INFO "%d reader incrementing the reader count\n",rw);
	read_count++;
	count++;
	if(read_count == 1){
		printk(KERN_INFO "Making writer to wait\n");
		down(&rw_mutex);
	}
	up(&mutex);	
	printk(KERN_INFO "%d is reading\n",rw);
	printk(KERN_INFO "value of a shared value is %d\n",shared_var);
	
	down(&mutex);
	printk(KERN_INFO "%d reader is leaving\n",rw);
	read_count--;
	if(read_count == 0){
		printk(KERN_INFO "%d reader signal writer to perform writing\n",rw);
		up(&rw_mutex);
	}
	up(&mutex);
	do_exit(0);
	return 0;
}

static int __init read_write(void){

	char th[8] = "thread0";
	int i=0;
	printk(KERN_INFO "\nInserting module for reader writer\n");
	printk(KERN_INFO "Initializing semaphore for writer\n");
	sema_init(&rw_mutex, 1);
	printk(KERN_INFO "Initializing semaphore\n");
	sema_init(&mutex, 1);

	printk(KERN_INFO "creating kernel threads\n");

	i=5;
	
	thread1 = kthread_create(write, i, th);
	if((thread1)){
		printk(KERN_INFO "waking up thread %d\n",i);
		wake_up_process(thread1);
	}

	for(i=0; i<parm_reader; i++){
		th[7] = (char)(32+i);
		thread2[i] = kthread_create(read, i, th);
		if((thread2[i])){
			printk(KERN_INFO "waking up thread %d\n",i);
			wake_up_process(thread2[i]);
		}
	}


	if(count >= 6){
		for(i =0; i<parm_reader; i++) 
			kthread_stop(thread2[i]);
	
		kthread_stop(thread1);
	}
	
	return 0;
}

static void __exit read_write_exit(void){

	printk(KERN_INFO "Cleaning up module\n");
	
}


module_param(rw,int, 0644);
module_param(read_count,int, 0644);
module_param(parm_reader,int, 0644);
module_param(parm_writer,int, 0644);

module_init(read_write);
module_exit(read_write_exit);

MODULE_AUTHOR("Talha Hussain & Anusha");
MODULE_DESCRIPTION("Reader and writer problem");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

