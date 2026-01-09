/*
 *    Description:  Linux kernel module source template
 *         Author:  fpktools, zxx@ashyelf.com
 *        Company:  ashyelf
 */

/* Common kernel module header files */
#include <linux/init.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/device.h>
#include "function.h"

/* Sample function */
void kernel_module_func( int c, int i )
{
    printk("fpktools kernel module sample function( %d, %d)\n", c, i );
}



/* Kernel module initialization function */
int __init module_register( void )
{
    printk("fpktools kernel module register\n");
    kernel_module_func( KERNEL_MODULE_MACRO_1, 20 );
    return 0;
}

/* Kernel module deregisters function */
void __exit module_unregister( void )
{
    printk("fpktools kernel module unregister\n");
    kernel_module_func( KERNEL_MODULE_MACRO_1, 40 );
}



/* Kernel module initialization function registration */
module_init(module_register);
/* Kernel module deregisters functions */
module_exit(module_unregister);

/* Kernel module information */
MODULE_AUTHOR("fpktools");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("fpktools kernel module generate by fpktools");

