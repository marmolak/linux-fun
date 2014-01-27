#include <linux/mm.h>
#include <linux/module.h>	/* Needed by all modules */
#include <linux/kernel.h>	/* Needed for KERN_INFO */
#include <linux/init.h>		/* Needed for the macros */
#include <linux/sched.h>    /* for task_struct */
#include <linux/pid.h>      /* for pid_task and find */
#include <linux/types.h>    /* for pid_t */
#include <linux/rcupdate.h> /* for rcu_read_lock */

MODULE_LICENSE("GPL");

static int __init madvise_swap_init(void)
{
    struct task_struct *task_to_swap;
    pid_t pid;
    struct vm_area_struct *vm_area;
    unsigned long vm_start;
    unsigned long vm_end;

    pid = 14799;

	printk(KERN_INFO "Try to get task struct for pid %u\n", pid);

    /* I maybe spinlock will be needed */
    rcu_read_lock();
    task_to_swap = pid_task(find_vpid(pid), PIDTYPE_PID);
    if (!task_to_swap) {
        rcu_read_unlock();
        printk (KERN_INFO "No task found!");
        return 0;
    }

    spin_lock(&task_to_swap->active_mm->page_table_lock);

    vm_area = task_to_swap->active_mm->mmap;
    while (vm_area != NULL) {
        vm_start = vm_area->vm_start;
        vm_end = vm_area->vm_end;
        if (!vm_area->vm_file) {
            printk(KERN_INFO "task_struct addr: %p %lx %lx\n", task_to_swap, vm_start, vm_end);
        }
        vm_area = vm_area->vm_next;
    }

    spin_unlock(&task_to_swap->active_mm->page_table_lock);
    rcu_read_unlock();

	return 0;
}

static void __exit madvise_swap_exit(void)
{
	printk(KERN_INFO "Goodbye, world 2\n");
}

module_init(madvise_swap_init);
module_exit(madvise_swap_exit);
