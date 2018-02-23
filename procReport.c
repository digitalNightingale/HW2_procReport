#include<linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>


int unrun = 0;
int run = 0;
int stop = 0;

void getRunables(struct task_struct *currentTask) {    // https://notes.shichao.io/lkd/ch3/
    struct task_struct *parentTaskPtr;  // pointer to parent task
    struct list_head *childList;        // list of children

    if (currentTask->state < 0) {       // state == -1 is unrunnable
      unrun++;
    }
    else if (currentTask->state == 0) { // state == 0 is runnable
      run++;
  } else { // state > 0 is stopped
      stop++;
    }
    list_for_each(childList, &currentTask->children) {
        parentTaskPtr = list_entry(childList, struct task_struct, sibling);
        getRunables(parentTaskPtr);
    }
}

void printProcesses(struct task_struct *currentTask) {     // source: https://linuxgazette.net/133/saha.html




  for_each_process(currentTask) {
      //printk(KERN_INFO "Process ID=%d\n", task->pid);
      //printk(KERN_INFO "Name=%s\n", task->comm);
      //printk(KERN_INFO "State of process=%ld]\n", task->state);
      printk(KERN_INFO "Process ID=%d Name=%s number_of_children=*not coded* **_child_pid=*not coded* **_child_name=*not coded*\n", currentTask->pid, currentTask->comm);
  }

}

int proc_init (void) {
  printk(KERN_INFO "helloModule: kernel module initialized\n");
  printk(KERN_INFO "PROCESS REPORTER\n");
  getRunables(&init_task);
  printk(KERN_INFO "Unrunnable:%d\nRunnable:%d\nStopped:%d\n", unrun, run, stop);
  printProcesses(&init_task);
  return 0;
}

void proc_cleanup(void) {
  printk(KERN_INFO "helloModule: performing cleanup of module\n");
}

MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);
