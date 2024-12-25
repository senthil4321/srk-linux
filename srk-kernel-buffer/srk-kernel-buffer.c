#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init kernel_buffer_init(void) {
    printk(KERN_INFO "SRK Kernel buffer module loaded.\n");
    return 0;
}

static void __exit kernel_buffer_exit(void) {
    printk(KERN_INFO "SRK Kernel buffer module unloaded.\n");
}

module_init(kernel_buffer_init);
module_exit(kernel_buffer_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Your Name");
MODULE_DESCRIPTION("A simple module to print messages to the kernel buffer.");
