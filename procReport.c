/* Norris Spencer
* Leah Ruisenor
*
* TCSS 422
* Winter 2018
* Home Work 2: procReport kernel
*/

#include<linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>
#include <linux/init.h>
#include <linux/proc_fs.h>
#include <linux/seq_file.h>
#include <linux/proc_fs.h>
#include <linux/sched/task.h>

int unrun = 0;
int run = 0;
int stop = 0;

/**
* This function itterates over the current processes and counts the states of each.
*
* Source: https://notes.shichao.io/lkd/ch3/
*
*/
void getRunables(struct task_struct *currentTask) {
    struct task_struct *parentTask;                    // pointer to parent task
    struct list_head *childList;                       // list of children

    if (currentTask->state < 0) {                      // state == -1 is unrunnable
        unrun++;
    } else if (currentTask->state == 0) {              // state == 0 is runnable
        run++;
    } else {                                           // state > 0 is stopped
        stop++;
    }
    // go through the current task list and count states recursivly
    list_for_each(childList, &currentTask->children) {
        parentTask = list_entry(childList, struct task_struct, sibling);
        getRunables(parentTask);
    }
}

/**
* This function prints the parent and child processes to the proc file.
*
* Source: https://linuxgazette.net/133/saha.html
*
*/
void printProcesses(struct seq_file *seqFile, struct task_struct *parentTask) {

    struct task_struct *childTask;
    struct list_head *childList;
    int children = 0;

    //for_each_process(parentTask) {
    //printk(KERN_INFO "Process ID=%d\n", parentTask->pid);
    //printk(KERN_INFO "Name=%s\n", parentTask->comm);
    //printk(KERN_INFO "State of process=%ld]\n", parentTask->state);
    list_for_each(childList, &parentTask->children) {
        children++;
    }
    // if there are no children processes print no children
    if (children == 0) {
        //printk(KERN_INFO "Process ID=%d Name=%s *No Children", parentTask->pid, parentTask->comm);
        seq_printf(seqFile, "Process ID=%d Name=%s *No Children\n", parentTask->pid, parentTask->comm);
        // else print out the number of children and the first child process
    } else {
        childTask = list_first_entry(childList, struct task_struct, sibling);
        //printk("Process ID=%d Name=%s number_of_children=%d first_child_pid=%d first_child_name=%s", parentTask->pid, parentTask->comm, children, childTask->pid, childTask->comm);
        seq_printf(seqFile, "Process ID=%d Name=%s number_of_children=%d first_child_pid=%d first_child_name=%s\n", parentTask->pid, parentTask->comm, children, childTask->pid, childTask->comm);
    }
    // recursivly go through the parent list to find each parent/child combo
    list_for_each(childList, &parentTask->children) {
        childTask = list_entry(childList, struct task_struct, sibling);
        printProcesses(seqFile, childTask);
    }
    // printk(KERN_INFO "Process ID=%d Name=%s number_of_children=*not coded* **_child_pid=*not coded* **_child_name=*not coded*\n", parentTask->pid, parentTask->comm);
    //}
}

/**
* This function calles the functions for the file printing.
*
* Source: http://stackover ow.com/questions/8516021/proc-create-example-for-kernel-module/
*
* @return 0  upon success
*/
static int proc_report_show(struct seq_file *seqFile, void *v) {

    //printk(KERN_INFO "PROCESS REPORTER\n");
    seq_printf(seqFile, "PROCESS REPORTER\n");

    getRunables(&init_task);

    //printk(KERN_INFO "Unrunnable:%d\nRunnable:%d\nStopped:%d\n", unrun, run, stop);
    seq_printf(seqFile, "Unrunnable:%d\nRunnable:%d\nStopped:%d\n", unrun, run, stop);

    printProcesses(seqFile, &init_task);

    return 0;
}

/**
* This function starts the opening of the proc file.
*
* Source: http://stackover ow.com/questions/8516021/proc-create-example-for-kernel-module/
*
* @return single_open which calls the proc_report_show.
*/
static int proc_report_open(struct inode *inode, struct  file *file) {
    return single_open(file, proc_report_show, NULL);
}

/**
* Constructor for the procfile.
*
* Source: http://stackover ow.com/questions/8516021/proc-create-example-for-kernel-module/
*
*/
static const struct file_operations proc_report_fops = {
    .owner = THIS_MODULE,
    .open = proc_report_open,
    .read = seq_read,
    .llseek = seq_lseek,
    .release = single_release,
};

/**
* This function is called when the module is loaded.
*
* @return 0  upon success
*/
int proc_init (void) {

    printk(KERN_INFO "procReport: kernel module initialized\n");
    proc_create("proc_report", 0, NULL, &proc_report_fops);
    printk("proc_report Created.\n");
    //     printk(KERN_INFO "PROCESS REPORTER\n");
    //     getRunables(&init_task);
    //     printk(KERN_INFO "Unrunnable:%d\nRunnable:%d\nStopped:%d\n", unrun, run, stop);
    //     seq_printf(m, "Unrunnable:%d\nRunnable:%d\nStopped:%d\n", unrun, run, stop);
    //     printProcesses(&init_task);
    return 0;
}

/**
* This function is called when the module is removed.
*/
void proc_cleanup(void) {
    remove_proc_entry("proc_report", NULL);
    printk(KERN_INFO "procReport: performing cleanup of module\n");
}

// Macros for registering module entry and exit points.
MODULE_LICENSE("GPL");
module_init(proc_init);
module_exit(proc_cleanup);
