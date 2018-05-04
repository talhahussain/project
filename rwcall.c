#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/semaphore.h>


struct semaphore rw_mutex;
struct semaphore mutex;


static int read_count =0;
static int rw=0;
static int shared_var=0;


int reader(void *no){

	int rw = (int) no;

	printk(KERN_INFO "%d reader is waiting\n",rw);
	down(&mutex);
	printk(KERN_INFO "%d reader incrementing the reader count\n",rw);
	read_count++;
	
	if(read_count == 1){
		printk(KERN_INFO "Making writer to wait\n");
		down(&rw_mutex);
	}
	up(&mutex);	
	printk(KERN_INFO "%d is reading\n",rw);
	printk(KERN_INFO "value of the variable is %d\n",shared_var);		
	down(&mutex);
	printk(KERN_INFO "%d reader is leaving\n",rw);
	read_count--;
	if(read_count == 0){
		printk(KERN_INFO "%d reader signal writer to perform writing\n",rw);
		up(&rw_mutex);
	}
	up(&mutex);
	return shared_var;

}
int writer(void *no){

	int rw = (int) no;
	printk(KERN_INFO "%d Writer is waiting\n",rw); 
	down(&rw_mutex);
	printk(KERN_INFO "%d is writring\n", rw);
	shared_var++;
	printk(KERN_INFO "%d writer is leaving\n", rw);
	up(&rw_mutex);
	return shared_var;

}
static int __init read_write(void){
	
	int a;
	printk(KERN_INFO "\nInserting module\n");
	sema_init(&rw_mutex, 1);
	sema_init(&mutex, 1);

	a = writer(0);
	a = reader(1);
	return 0;
}



static void __exit read_write_exit(void){

	printk(KERN_INFO "Cleaning up module\n");

}


module_param(rw,int, 0644);
module_param(read_count,int,0644);
module_param(shared_var,int,0644);

module_init(read_write);
module_exit(read_write_exit);

MODULE_AUTHOR("Talha Hussain");
MODULE_DESCRIPTION("Reader and writer problem");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

