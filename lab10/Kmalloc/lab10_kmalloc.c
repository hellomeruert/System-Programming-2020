#include <linux/module.h>
#include <linux/random.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/kthread.h>
#include <linux/time.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/mm_types.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>

#define TAG "Kmalloc"
#define PAGE_SIZE 4096
MODULE_LICENSE("GPL");

struct my_struct {
    unsigned int elements[8];
}

static struct task_struct *t1;
static struct task_struct *t2;
static unsigned int parameter = 2000;

module_param(parameter, int, 0644);


void print_time(struct timespec *t) {
    printk("time: %lu (%.2lu:%.2lu:%lu)\n",
                    t->tv_nsec,
                   (t->tv_sec / 3600 + 6) % (24),
                   (t->tv_sec / 60) % (60),
                    t->tv_sec % 60);
}

static int threading(void *args) {
    struct timespec current_time, end_time;
    struct my_struct m;
    printk("Module parameter: %d", parameter);
    printk("Size of my struct: %ld", sizeof(m));
    getnstimeofday(&current_time);
    print_time(&current_time);
    int i;
    char *buf;
    for(i = 0; i < parameter; i++) {
        buf = kmalloc(sizeof(m), GFP_KERNEL);
        kfree(buf);
    }
    getnstimeofday(&end_time);
    print_time(&end_time);


    printk("Time spent: %lu seconds %lu nanoseconds", end_time.tv_sec - current_time.tv_sec, end_time.tv_nsec - current_time.tv_nsec);
    return 0;
}

int init_module(void)
{
    printk(KERN_INFO "START %s\n", TAG);
    t1 = kthread_run(threading, NULL, "first");
    t2 = kthread_run(threading, NULL, "second");
    kthread_stop(t2);
    return 0;
}

void cleanup_module(void)
{
    printk(KERN_INFO "END %s\n", TAG);
}
