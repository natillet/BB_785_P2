#include <linux/module.h>
#include <linux/kernel.h>

int interrupt_setup;

static int __init enable_usermode_init(void)
{
	/* enable user-mode access to the performance counter*/
	asm ("MCR p15, 0, %0, C9, C14, 0\n\t" :: "r"(1));
	/* read counter overflow interrupts (just in case)*/
	asm ("MRC p15, 0, %0, C9, C14, 2\n\t" :: "r"(interrupt_setup));
	/* disable counter overflow interrupts (just in case)*/
//	asm ("MCR p15, 0, %0, C9, C14, 2\n\t" :: "r"(0x8000000f));
	asm ("MCR p15, 0, %0, C9, C14, 2\n\t" :: "r"(interrupt_setup));
//	printk(KERN_INFO "User mode enabled.\n");
	printk(KERN_INFO "User mode enabled: overflow interrupt setup is %x\n", interrupt_setup);
	return 0;
}

static void __exit exit_usermode(void)
{
	/* disable user-mode access to the performance counter*/
	asm ("MCR p15, 0, %0, C9, C14, 0\n\t" :: "r"(0));
	/* enable counter overflow interrupts (just in case)*/
//	asm ("MCR p15, 0, %0, C9, C14, 1\n\t" :: "r"(0x8000000f));
	asm ("MCR p15, 0, %0, C9, C14, 1\n\t" :: "r"(interrupt_setup));
	printk(KERN_INFO "User mode disabled.\n");
}

module_init(enable_usermode_init);
module_exit(exit_usermode);
