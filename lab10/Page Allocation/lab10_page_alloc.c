#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/kthread.h>
#include <linux/time.h>
#include <linux/gfp.h>
#include <linux/mm_types.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>

#define TAG "Page Allocation"
#define PAGE_SIZE 4096
MODULE_LICENSE("GPL");

struct my_struct {
    unsigned int elements[8];
};

struct page* pages;
static struct task_struct *t1;
static struct task_struct *t2;
static unsigned int parameter = 2000;

module_param(parameter, int, 0644);

void print_time(struct timespec *t) {
    printk("TIME: %lu (%.2lu:%.2lu:%lu)\n",
                    t->tv_nsec,
                   (t->tv_sec / 3600 + 6) % (24),
                   (t->tv_sec / 60) % (60),
                    t->tv_sec % 60);
}

static int thread_function(void *args) {
    struct timespec current_time, end_time;
    struct my_struct m;
    int number_of_pages = 1, pow = 0;
    while (number_of_pages * PAGE_SIZE/sizeof(m) < parameter){
        number_of_pages *= 2;
        pow++;
    }
    printk("Module parameter: %d", parameter);
    
    printk("Struct size: %ld", sizeof(m));
    printk("Amount of struct elements in 1 memory page: %ld", PAGE_SIZE/sizeof(m));
    printk("Amount of pages(module param): 2^%d = %d", pow,number_of_pages);
    getnstimeofday(&current_time);
    print_time(&current_time);
    pages = alloc_pages(GFP_KERNEL,pow);
    __free_pages(pages, pow);
    getnstimeofday(&end_time);
    print_time(&end_time);

    printk("Time spent: %lu seconds %lu nanoseconds", end_time.tv_sec - current_time.tv_sec, end_time.tv_nsec - current_time.tv_nsec);

    return 0;

}

int init_module(void)
{
    printk(KERN_INFO "START %s\n", TAG);
    t1 = kthread_run(thread_function, NULL, "first");
    t2 = kthread_run(thread_function, NULL, "second");
    kthread_stop(t2);
    return 0;
}

void cleanup_module(void)
{
    printk(KERN_INFO "END %s\n", TAG);
}
