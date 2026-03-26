/*
 * ============================================================================
 * Linux kernel module template (.ko via Kbuild)
 * ============================================================================
 *
 * This tree is independent of skin/skin.h.  Integrate through your BSP / Linux
 * kernel package Makefile (out-of-tree or in-tree).
 *
 * Current Kbuild produces mod.ko from module.c (see Makefile header to rename).
 * ============================================================================
 */

#include <linux/init.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/interrupt.h>
#include <linux/fs.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/device.h>

#include "module.h"

void kernel_module_func( int c, int i )
{
	printk( KERN_INFO "tmptools kmodule sample: c=%d i=%d\n", c, i );
}

static int __init tmptools_mod_init( void )
{
	printk( KERN_INFO "tmptools kmodule: init\n" );
	kernel_module_func( KERNEL_MODULE_MACRO_SAMPLE, 20 );
	return 0;
}

static void __exit tmptools_mod_exit( void )
{
	printk( KERN_INFO "tmptools kmodule: exit\n" );
	kernel_module_func( KERNEL_MODULE_MACRO_SAMPLE, 40 );
}

module_init( tmptools_mod_init );
module_exit( tmptools_mod_exit );

MODULE_AUTHOR( "Skinos template — replace" );
MODULE_LICENSE( "GPL" );
MODULE_DESCRIPTION( "Skinos tmptools kernel module template" );
