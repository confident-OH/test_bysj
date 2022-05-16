/* 
 * virtio_htc_ioctl.c: 
       Create an input/output character device to 
       link kernel and user space
 */ 

#include <linux/delay.h> 
#include <linux/device.h> 
#include <linux/fs.h> 
#include <linux/init.h> 
#include <linux/irq.h> 
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/poll.h>
#include <linux/notifier.h>

#define SUCCESS 0 

extern struct raw_notifier_head virtio_htc_othermod[10];

int virtio_htc_mod_test2_notifier_event(struct notifier_block *nb, unsigned long event, void *v)
{
    char *item = (char *)v;
    printk("mod test2 do command: %s\n", item);
    return SUCCESS;
}

// notifier block
static struct notifier_block virtio_htc_mod_test2_notifier = {
    .notifier_call = virtio_htc_mod_test2_notifier_event, 
};
 
/* Initialize the module - Register the character device */ 
static int __init virtio_htc_mod_test2_init(void) 
{ 
    int ret_val = raw_notifier_chain_register(&virtio_htc_othermod[1], &virtio_htc_mod_test2_notifier);
    if (ret_val)
    {
        printk("error mod test2\n");
        return -1;
    }
    return ret_val; 
} 
 
/* Cleanup - unregister the appropriate file from /proc */ 
static void __exit virtio_htc_mod_test2_exit(void) 
{ 
    raw_notifier_chain_unregister(&virtio_htc_othermod[1], &virtio_htc_mod_test2_notifier);
} 
 
module_init(virtio_htc_mod_test2_init); 
module_exit(virtio_htc_mod_test2_exit);
 
MODULE_DESCRIPTION("Virtio htc mod test2");
MODULE_LICENSE("GPL");