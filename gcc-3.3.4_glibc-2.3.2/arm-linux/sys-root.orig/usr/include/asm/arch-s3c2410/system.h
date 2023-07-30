/*
 *  linux/include/asm-arm/arch-s3c2410/system.h
 *
 *  Copyright (C) 2002 SAMSUNG ELECTRONICS
 *                       SW.LEE  <hitchcar@sec.samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */
#ifndef __ASM_ARCH_SYSTEM_H
#define __ASM_ARCH_SYSTEM_H

#include <asm/hardware.h>
#include <asm/io.h>

/* 
 * This functions used in arm/kernel/process.c 
 */

#ifdef CONFIG_LEDS
extern void Led_Display(int);
#endif

static void arch_idle(void)
{
	/*
	 * This should do all the clock switching
	 * and wait for interrupt tricks
	 */
#ifndef CONFIG_LEDS
	cpu_do_idle(0);
#else
	int i;

	//Led_Display(1);
	rCLKCON |= (1<<2);	/* Enter IDLE Mode */
	for ( i = 0; i <100;i++);
	rCLKCON &= ~(1<<2);
	//Led_Display(4);
#endif

}

static inline void arch_reset(char mode)
{

   	/* use the watchdog timer reset to reset the processor */
 
 	/* (at this point, MMU is shut down, so we use physical addrs) */
 	volatile unsigned long *prWTCON = (unsigned long*) (PA_WD_BASE + 0x00);
 	volatile unsigned long *prWTDAT = (unsigned long*) (PA_WD_BASE + 0x04);
 	volatile unsigned long *prWTCNT = (unsigned long*) (PA_WD_BASE + 0x08);

    cli ();                 /* disable all the interrupts */
	clf ();

 	/* set the countdown timer to a small value before enableing WDT */
//	rGSTATUS2 = 2;
 	*prWTDAT = 0x00000100;
 	*prWTCNT = 0x00000100;
 
 	/* enable the watchdog timer */
 	*prWTCON = 0x00008021;
 
//    rGSTATUS2 = 2;  

 	/* machine should reboot..... */
 
 	mdelay(5000);
 	panic("Watchdog timer reset failed!\n");
 	printk(" Jump to address 0 \n");
 	cpu_reset(0);
}

#endif
