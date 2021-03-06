#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>

#define TAG "week10"

static struct timer_list mytimer;

void exampleTimerFunc(struct timer_list *t) {
   printk(KERN_INFO "My timer has been executed!\n");
}

void exampleWithTimer(void) {
   printk( "Starting timer to fire in 1000ms (%ld)\n", jiffies );
   mytimer.expires = jiffies + msecs_to_jiffies(1000);
   timer_setup(&mytimer, exampleTimerFunc, 0);
   add_timer(&mytimer);
}

int init_module(void) {
   printk(KERN_INFO "Starting %s\n", TAG);
   exampleWithTimer();
   return 0;
}

void cleanup_module(void) {
   printk(KERN_INFO "Cleanup %s\n", TAG);
   del_timer(&mytimer);
}
