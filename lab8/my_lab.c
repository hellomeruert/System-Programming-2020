#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/random.h>
#include <linux/kthread.h>
#include <linux/sched.h>
#include <linux/spinlock.h>
#define TAG "Week 10"
#define MAX_NUM 1000

int L[MAX_NUM]; //unsorted
int R[MAX_NUM]; //sorted 

DEFINE_SPINLOCK(lock);
struct mergeSP {
    int from;
    int m;
    int end;
};

struct ts_par {
    int start;
    int end;
};

static struct task_struct *t1;
static struct task_struct *t2;
static struct task_struct *t3;
static struct task_struct *t4;

int random_vars(void) {
    int n;
    get_random_bytes(&n, sizeof(n));
    n = n < 0 ? n * -1 : n;
    n = n % MAX_NUM;
    return n;
}

void fillArray(int size) {
    int i;
    for (i=0; i<size; i++)
        L[i] = random_vars();
}
void printArray(int *list, int size) {
    int i;
    printk("first 10 elements");
     for (i=0; i<10; i++)
        printk("%d\n", list[i]); 
    printk("last 10 elements");
    for (i=size-11; i<size-1; i++)
        printk("%d\n", list[i]);  
    printk("\n");
}

static int threadSort(void *args) {
    spin_lock(&lock);
    struct ts_par *p = (struct ts_par*) args;
    int start = p->start, end = p->end;
   
    int i = start, j, it;

    while (i <= end) {
        it = L[i]; 
        j = i-1;
        while (j >= start && it < L[j])
        {
            L[j+1] = L[j];
            j--;
        }
        L[j+1] = it;
        i++;
    }
    spin_unlock(&lock);
    return 0;
}
static int merge(void *args) {
    spin_lock(&lock);
    struct mergeSP *p = (struct mergeSP*) args;
    int from = p->from, m = p->m, end = p->end;
    int i = from, j = m, pos = from;

    while (i < m && j <= end)
    {
        if (L[i] < L[j])
            R[pos++] = L[i++];
        else
            R[pos++] = L[j++];
    }

    while (i < m)
        R[pos++] = L[i++];
    while (j <= end)
        R[pos++] = L[j++];
    spin_unlock(&lock);
    printArray(R, MAX_NUM);
    return 0;
}

int init_module(void)
{
    printk(KERN_INFO "START %s\n", TAG);
    fillArray(MAX_NUM);
    printArray(L, MAX_NUM);

    struct ts_par _args[2];
    _args[0].start = 0;
    _args[0].end = MAX_NUM/2;
    _args[1].start = MAX_NUM/2 + 1;
    _args[1].end = MAX_NUM -1;

    struct mergeSP m_args;
    m_args.from = 0;
    m_args.m = MAX_NUM/2 + 1;
    m_args.end = MAX_NUM - 1;
    t1 = kthread_run(threadSort, &_args[0], "Thread 1");
    
    t2 = kthread_run(threadSort, &_args[1], "Thread 2");
   
    t3 = kthread_run(merge, &m_args, "Thread 3");
  
    t4 = kthread_run(merge, &m_args, "Thread 4");
    kthread_stop(t1);
    kthread_stop(t2);
    kthread_stop(t3);
    kthread_stop(t4);
    return 0;
}

void cleanup_module(void)
{
        printk(KERN_DEBUG "END %s\n", TAG);
}