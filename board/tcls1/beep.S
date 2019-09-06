#include <config.h>
#include <ppc4xx.h>

#define _LINUX_CONFIG_H 1	/* avoid reading Linux autoconf.h file	*/
#define FPGA_BRDC       0xF0300004

#include <ppc_asm.tmpl>
#include <ppc_defs.h>

#include <asm/cache.h>
#include <asm/mmu.h>

/* This file is compiled twice:	once as little endian, once as
   big endian. As long as there are no memory references,
   this should be ok. */

/* Which outputs are driven by bits in the GPIO register
   rather than the alternate I/O assignment. See board layout
 */

#include "gpio.h"

	.globl  gpio_init

 /* destroys r3, r4 */
gpio_init:

/* review with JH ?? */
/* GPIO register base. */
	addis	r3, 0, 0xef60
	ori	r3, r3, 0x0700

	li	r4, 0
	stw	r4, GPIO0_OFF_RR1 (r3)
	stw	r4, GPIO0_OFF_OR (r3)

/* select output sources: Very board dependent. */
	addis	r4, 0, 0x5555       /* GPIO0-GPIO7 from alternate */
	ori     r4, r4, 0x4000      /* GPIO8 alternate, GPIO9-GPIO15 from register */
	stw	r4, GPIO0_OFF_OSRH (r3)

	addis	r5, 0, 0x1550       /* GPIO16, GPIO22 and GPIO23 from reg, GPIO17-GPIO21 from alternate */
	ori     r5, r5, 0x0051      /* GPIO28, GPIO29 and GPIO31 alternate, others from register */
	stw	r5, GPIO0_OFF_OSRL (r3)

/* selecting input sources: Alternative inputs (such as IRQs) are
   driven from pin, all others from RR (which contains all zeroes) */

	stw	r4, GPIO0_OFF_ISR1H (r3)
	stw	r5, GPIO0_OFF_ISR1L (r3)

/* All input pins have 0 .. these are: GPIO17-22, 24-26 */

	addis	r4, 0, 0xffff
	ori	r4, r4, 0x811f
	stw	r4, GPIO0_OFF_TCR (r3)

/* Three state select register: completely senseless on the
   NP405L, since there is only one source (the TCR) anyway */

	addis	r4, 0, 0x0
	ori	r4, r4, 0x0
	stw	r4, GPIO0_OFF_TSRH (r3)
	stw	r4, GPIO0_OFF_TSRL (r3)

/* Only GPIO22 is open drain .. */

	addis	r4, 0, 0x0000
	ori	r4, 0, 0x0200
	stw	r4, GPIO0_OFF_ODR (r3)

	blr


/* This beeps for approx. 200ms (at 200MHz clock), followed by
   a 200ms delay. Used for debugging purposes mainly, destroys
   r3, r4. Note: beeper attached to the GPIO pin 27, so
   be sure to initialize GPIO first.  */

     	.globl	beep
  /* make sure to initialize gpio first .. */
beep:

/* GPIO register base. */
	addis	r30, 0, 0xef60
	ori	r30, r30, 0x0700

/* not completely clear, probably its better to take this
   from the input register .. */

	lwz	r31, GPIO0_OFF_OR(r30)
	ori	r31, r31, 0x0010
	stw	r31, GPIO0_OFF_OR(r30)

/* approximately 0.2 sec sound .. */

	addis	r31,0,0x0080
        ori     r31,r31,0x0000
        mtctr   r31
..spinlp1:
        bdnz    ..spinlp1                // spin loop

	lwz	r31, GPIO0_OFF_OR(r30)
	andi.	r31, r31, 0xffef
	stw	r31, GPIO0_OFF_OR(r30)

/* .. and 0.2 sec no sound.. */
	addis	r31,0,0x0040
        ori     r31,r31,0x0000
        mtctr   r31
..spinlp2:
        bdnz    ..spinlp2                // spin loop

        blr

    	.globl	ledon
  /* make sure to initialize gpio first .. */
ledon:

/* GPIO register base. */
	addis	r30, 0, 0xef60
	ori	r30, r30, 0x0700

/* not completely clear, probably its better to take this
   from the input register .. */

	lwz	r31, GPIO0_OFF_OR(r30)
	ori	r31, r31, 0x0002
	stw	r31, GPIO0_OFF_OR(r30)
        blr

    	.globl	ledoff
  /* make sure to initialize gpio first .. */
ledoff:

	lwz	r31, GPIO0_OFF_OR(r30)
	andi.	r31, r31, 0xfffd
	stw	r31, GPIO0_OFF_OR(r30)

        blr



