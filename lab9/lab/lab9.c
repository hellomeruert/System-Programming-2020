#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/hrtimer.h>
#include <linux/ktime.h>

#define TAG "Lab 9"
#define MS_TO_NS(x)     (x * 1E6L)

MODULE_LICENSE("GPL");

static struct timer_list mytimer;
static struct hrtimer hr_timer;
static int fibnum;

unsigned long timer_interval_ns = 1e6;
static struct hrtimer hr_timer;

void exampleTimerFunc(struct timer_list *t) {
   printk(KERN_INFO "Simple timer Time is %lld \n", jiffies);
   mytimer.expires = jiffies + msecs_to_jiffies(1000);
   timer_setup(&mytimer, exampleTimerFunc, 0);
   add_timer(&mytimer);
}

int fib(int n) 
{ 
   if (n <= 1) 
      return n; 
   return fib(n-1) + fib(n-2); 
} 

enum hrtimer_restart fibTF_callback( struct hrtimer *timer )
{
    ktime_t kt,currtime;

    int n;
      
    unsigned long delay_in_ms = 1e9;
    n = fibnum;
      
    currtime  = ktime_get();
    kt = ktime_set( 0, delay_in_ms * (unsigned long) fib(n));
    
    hrtimer_forward(timer, timer->_softexpires, kt);

    fibnum = n + 1;
  printk( "fibTF_callback called (%ld). With fibnum: %d \n", jiffies, fib(n));
    
  return HRTIMER_RESTART;
}

void exampleWithTimer(void) {
    ktime_t ktime;

   printk( "Starting timer to fire in 1000ms (%ld)\n", jiffies );
   mytimer.expires = jiffies + msecs_to_jiffies(1000);
   timer_setup(&mytimer, exampleTimerFunc, 0);


    unsigned long delay_in_ms = 1L;

    printk("hrtimer module installing\n");

    ktime = ktime_set( 0, MS_TO_NS(delay_in_ms) );

    hrtimer_init( &hr_timer, CLOCK_MONOTONIC, HRTIMER_MODE_REL );
    
    hr_timer.function = &fibTF_callback;

    printk( "Starting timer to fire in %ldms (%ld)\n", 
    delay_in_ms, jiffies );

    hrtimer_start( &hr_timer, ktime, HRTIMER_MODE_REL );

    add_timer(&mytimer);

}

int init_module(void) {
   printk(KERN_INFO "Starting %s\n", TAG);
   exampleWithTimer();
   return 0;
}

void cleanup_module(void) {
    int ret;
   printk(KERN_INFO "Cleanup %s\n", TAG);
   del_timer(&mytimer);

  ret = hrtimer_cancel( &hr_timer );
  if (ret) printk("The timer was still in use...\n");

  printk("hrtimer module removing\n");
}

   