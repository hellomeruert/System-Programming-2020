#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/kthread.h>
#include <linux/time.h>
#include <linux/gfp.h>
#include <linux/slab.h>
#include <linux/mm_types.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/moduleparam.h>

#define TAG "Slab Allocation"
#define PAGE_SIZE 4096


MODULE_LICENSE("GPL");

struct my_struct {
    unsigned int elements[8];
};

static struct task_struct *t1;
static struct task_struct *t2;
struct my_struct m;
static struct kmem_cache *km;
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
    struct timespec cur_time, end_time;
    
    printk("Module parameter: %d", parameter);
    printk("Struct size: %ld", sizeof(m));
    getnstimeofday(&cur_time);
    print_time(&cur_time);
    int i;
    char *buf;
    for(i = 0; i < parameter; i++) {
        buf = kmem_cache_alloc(km, GFP_KERNEL);
        kmem_cache_free(km, buf);
    }
    getnstimeofday(&end_time);
    print_time(&end_time);

    printk("Time spent: %lu seconds %lu nanoseconds", end_time.tv_sec - cur_time.tv_sec, 
        end_time.tv_nsec - cur_time.tv_nsec);

    return 0;

}

int init_module(void)
{
    printk(KERN_INFO "START %s\n", TAG);
    km = kmem_cache_create("slab", sizeof(m), 0, SLAB_HWCACHE_ALIGN|SLAB_POISON|SLAB_RED_ZONE, NULL);
    t1 = kthread_run(thread_function, NULL, "first");
    t2 = kthread_run(thread_function, NULL, "second");
    kthread_stop(t2);
    return 0;
}

void cleanup_module(void)
{
    kmem_cache_destroy(km);
    printk(KERN_INFO "END %s\n", TAG);
}
