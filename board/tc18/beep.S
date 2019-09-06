#include <asm/processor.h>
#include <ppc_asm.tmpl>
#include <config.h>
#include <ppc440.h>


#define GPIO0_OFF_OR 	0x0

/* This file is compiled twice:	once as little endian, once as
   big endian. As long as there are no memory references,
   this should be ok. */

/* Which outputs are driven by bits in the GPIO register
   rather than the alternate I/O assignment. See board layout
 */

	.globl  asm_gpio_init

 /* destroys r3, r4 */
asm_gpio_init:


	addis	r3, 0, GPIO0_OR@h
	ori	r3, r3, GPIO0_OR@l
	li	r4, 0
	stw	r4,0x0000(r3)


	addis	r3, 0, GPIO0_TCR@h
	ori	r3, r3, GPIO0_TCR@l
	addis	r4, 0, 0x2010
	ori	r4, r4, 0xf18f
	stw	r4,0x0000(r3)


	addis	r3, 0, GPIO0_ODR@h
	ori	r3, r3, GPIO0_ODR@l
	li	r4, 0
	stw	r4,0x0000(r3)

	blr


/* This beeps for approx. 200ms (at 200MHz clock), followed by
   a 200ms delay. Used for debugging purposes mainly, destroys
   r3, r4. Note: beeper attached to the GPIO pin 11, so
   be sure to initialize GPIO first.  */
       .globl	asm_beep
  /* make sure to initialize gpio first .. */
asm_beep:

/* GPIO register base. */
	addis	r30, 0, 0xef60
	ori	r30, r30, 0x0b00

/* not completely clear, probably its better to take this
   from the input register .. */

	addis	r31, 0, 0x0010
	ori	r31, r31, 0x0000 
	/*lwz	r31, GPIO0_OFF_OR(r30)
	ori	r31, r31, 0x0010*/
	stw	r31, GPIO0_OFF_OR(r30)

/* approximately 0.2 sec sound .. */

	addis	r31,0,0x0080
        ori     r31,r31,0x0000
        mtctr   r31
..spinlp1:
        bdnz    ..spinlp1                // spin loop
	
	addis	r31, 0, 0xffef
	ori	r31, r31, 0xffff
	/*lwz	r31, GPIO0_OFF_OR(r30)
	andi.	r31, r31, 0xffff*/
	stw	r31, GPIO0_OFF_OR(r30)
	

/* .. and 0.2 sec no sound.. */
	addis	r31,0,0x0040
        ori     r31,r31,0x0000
        mtctr   r31
..spinlp2:
        bdnz    ..spinlp2                // spin loop

        blr
	
        .globl	asm_beep_on
  /* make sure to initialize gpio first .. */
asm_beep_on:

/* GPIO register base. */
	addis	r30, 0, 0xef60
	ori	r30, r30, 0x0b00

/* not completely clear, probably its better to take this
   from the input register .. */

	addis	r31, 0, 0x0010
	ori	r31, r31, 0x0000 
	/*lwz	r31, GPIO0_OFF_OR(r30)
	ori	r31, r31, 0x0010*/
	stw	r31, GPIO0_OFF_OR(r30)

	blr 
	
        .globl	asm_beep_off
  /* make sure to initialize gpio first .. */
asm_beep_off:

/* GPIO register base. */
	addis	r30, 0, 0xef60
	ori	r30, r30, 0x0b00

/* not completely clear, probably its better to take this
   from the input register .. */

	addis	r31, 0, 0xffef
	ori	r31, r31, 0xffff 
	/*lwz	r31, GPIO0_OFF_OR(r30)
	ori	r31, r31, 0x0010*/
	stw	r31, GPIO0_OFF_OR(r30)

	blr 

	






