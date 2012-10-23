#include <linux/module.h>
#include <linux/kernel.h>

static int __init enable_usermode_init(void)
{
	/* enable user-mode access to the performance counter*/
	asm ("MCR p15, 0, %0, C9, C14, 0\n\t" :: "r"(1));
	printk(KERN_INFO "User mode enabled.\n");
	return 0;
}

static void __exit exit_usermode(void)
{
	/* disable counter overflow interrupts (just in case)*/
	asm ("MCR p15, 0, %0, C9, C14, 2\n\t" :: "r"(0x8000000f));
	printk(KERN_INFO "User mode disabled.\n");
}

module_init(enable_usermode_init);
module_exit(exit_usermode);
