/*
 * (C) Copyright 2000
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * See file CREDITS for list of people who contributed to this
 * project.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
//------------------------------------------------------------------------------+
//
//       This source code has been made available to you by IBM on an AS-IS
//       basis.  Anyone receiving this source is licensed under IBM
//       copyrights to use it in any way he or she deems fit, including
//       copying it, modifying it, compiling it, and redistributing it either
//       with or without modifications.  No license under IBM patents or
//       patent applications is to be implied by the copyright license.
//
//       Any user of this software should understand that IBM cannot provide
//       technical support for this software and will not be responsible for
//       any consequences resulting from the use of this software.
//
//       Any person who transfers this source code or any derivative work
//       must include the IBM copyright notice, this paragraph, and the
//       preceding two paragraphs in the transferred software.
//
//       COPYRIGHT   I B M   CORPORATION 1995
//       LICENSED MATERIAL  -  PROGRAM PROPERTY OF I B M
//-------------------------------------------------------------------------------

#if (!defined __ASSEMBLY__)
#define __ASSEMBLY__
#endif


#include "ppc_asm.tmpl"
#include "ppc_defs.h"





#if defined(CONFIG_440)
#define CFG_PERIPHERAL_BASE     0xef600000

#if defined(CONFIG_440EP) || defined(CONFIG_440GR)
#define UART0_BASE  CFG_PERIPHERAL_BASE + 0x00000300
#define UART1_BASE  CFG_PERIPHERAL_BASE + 0x00000400
#else
#define UART0_BASE  CFG_PERIPHERAL_BASE + 0x00000200
#define UART1_BASE  CFG_PERIPHERAL_BASE + 0x00000300
#endif
#if defined(CONFIG_440GX) || defined(CONFIG_440EP) || defined(CONFIG_440GR)
#define CR0_MASK        0xdfffffff
#define CR0_EXTCLK_ENA  0x00800000
#define CR0_UDIV_POS    0
#else
#define CR0_MASK        0x3fff0000
#define CR0_EXTCLK_ENA  0x00600000
#define CR0_UDIV_POS    16
#endif /* CONFIG_440GX */
#elif defined(CONFIG_405L)
	#define UART0_BASE      0xef600300
	#define UART1_BASE      0xef600400
	#define UCR0_MASK       0x0000007f
	#define UCR1_MASK       0x00007f00
	#define UCR0_UDIV_POS   0
	#define UCR1_UDIV_POS   8
	#define UDIV_MAX        127
#elif defined(CONFIG_405EP)
#define UART0_BASE      0xef600300
#define UART1_BASE      0xef600400
#define UCR0_MASK       0x0000007f
#define UCR1_MASK       0x00007f00
#define UCR0_UDIV_POS   0
#define UCR1_UDIV_POS   8
#define UDIV_MAX        127
#else /* CONFIG_405GP || CONFIG_405CR */
#define UART0_BASE      0xef600300
#define UART1_BASE      0xef600400
#define CR0_MASK        0x00001fff
#define CR0_EXTCLK_ENA  0x000000c0
#define CR0_UDIV_POS    1
#define UDIV_MAX        32
#endif

/* using serial port 0 or 1 as U-Boot console ? */
#if defined(CONFIG_UART1_CONSOLE)
#define ACTING_UART0_BASE	UART1_BASE
#define ACTING_UART1_BASE	UART0_BASE
#if defined(CONFIG_440GX) || defined(CONFIG_440EP) || defined(CONFIG_440GR)
#define UART0_SDR           sdr_uart1
#define UART1_SDR           sdr_uart0
#endif /* CONFIG_440GX */
#else
#define ACTING_UART0_BASE	UART0_BASE
#define ACTING_UART1_BASE	UART1_BASE
#if defined(CONFIG_440GX) || defined(CONFIG_440EP) || defined(CONFIG_440GR)
#define UART0_SDR           sdr_uart0
#define UART1_SDR           sdr_uart1
#endif /* CONFIG_440GX */
#endif

#if defined(CONFIG_405EP) && defined(CFG_EXT_SERIAL_CLOCK)
#error "External serial clock not supported on AMCC PPC405EP!"
#endif

#define UART_RBR    0x00
#define UART_THR    0x00
#define UART_IER    0x01
#define UART_IIR    0x02
#define UART_FCR    0x02
#define UART_LCR    0x03
#define UART_MCR    0x04
#define UART_LSR    0x05
#define UART_MSR    0x06
#define UART_SCR    0x07
#define UART_DLL    0x00
#define UART_DLM    0x01

/*-----------------------------------------------------------------------------+
  | Line Status Register.
  +-----------------------------------------------------------------------------*/
/*#define asyncLSRport1           ACTING_UART0_BASE+0x05 */
#define asyncLSRDataReady1            0x01
#define asyncLSROverrunError1         0x02
#define asyncLSRParityError1          0x04
#define asyncLSRFramingError1         0x08
#define asyncLSRBreakInterrupt1       0x10
#define asyncLSRTxHoldEmpty1          0x20
#define asyncLSRTxShiftEmpty1         0x40
#define asyncLSRRxFifoError1          0x80



#define CPC0_UCR	0xf5		/* UART Control register */

#if 0

#define UART0_BASE  0xef600300
#define UART1_BASE  0xef600400

#define UART_RBR    0x00
#define UART_THR    0x00
#define UART_IER    0x01
#define UART_IIR    0x02
#define UART_FCR    0x02
#define UART_LCR    0x03
#define UART_MCR    0x04
#define UART_LSR    0x05
#define UART_MSR    0x06
#define UART_SCR    0x07
#define UART_DLL    0x00
#define UART_DLM    0x01

/*-----------------------------------------------------------------------------+
| Line Status Register.
+-----------------------------------------------------------------------------*/
#define asyncLSRDataReady1            0x01
#define asyncLSROverrunError1         0x02
#define asyncLSRParityError1          0x04
#define asyncLSRFramingError1         0x08
#define asyncLSRBreakInterrupt1       0x10
#define asyncLSRTxHoldEmpty1          0x20
#define asyncLSRTxShiftEmpty1  defined(CONFIG_440GR)       0x40
#define asyncLSRRxFifoError1          0x80

#endif


/*
 * Minimal serial functions needed to use one of the SMC ports
 * as serial console interface.
 */



	.text
	
	.global	serial_init_asm

/* Initializes UART to 115200 baud (formerly 19200 this was changed in
   1.08 which I think is a bad idea .. ) */

serial_init_asm:

  /* Disable DMA, set CPU::UART clock divisor to 14 */
#ifdef CONFIG_405L
	li	r3, 0x0e0e
	mtdcr	CPC0_UCR, r3
#elif defined(CONFIG_440GR)


	li	r3, UART0_SDR@l
	oris	r3, r3, UART0_SDR@h
	mtdcr	sdrcfga,r3

	li	r3, 0x800001@l
	oris	r3, r3, 0x800001@h
	mtdcr	sdrcfgd,r3
	
	

#endif

	li	r5, UART0_BASE@l
	oris	r5, r5, UART0_BASE@h

	li	r3, 0x80
	stb	r3, UART_LCR(r5)	/* set DLAB bit */

#ifdef CONFIG_405L
#if (CONFIG_BAUDRATE == 19200) 
	li	r3, 35
	stb	r3, UART_DLL(r5)	/* set divisor for 19200 baud */
#else  /* assumes 115200 */
	li	r3, 6
	stb	r3, UART_DLL(r5)	/* set divisor for 115200 baud */
#endif 

#elif defined(CONFIG_440GR)
	li	r3, 0x48
	stb	r3, UART_DLL(r5)	/* set divisor for 9600 baud */
	
#endif

	li	r3, 0x00
	stb	r3, UART_DLM(r5)

	li	r3, 0x03		/* 8 bits, no parity, 1 stop bit */
	stb	r3, UART_LCR(r5)

	li	r3, 0x0
	stb	r3, UART_FCR(r5)        /* disable FIFO */

	li	r3, 0x0
	stb	r3, UART_MCR(r5)        /* no modem control DTR RTS */

	lbz	r3, UART_LSR(r5)	/* clear line status */
	lbz	r3, UART_RBR(r5)	/* read reserial_divsceive buffer */

	li	r3, 0x0
	stb	r3, UART_SCR(r5)        /* set scratchpad */

	li	r3, 0x0e
	stb	r3, UART_IER(r5)        /* disable all interrupts */

	blr


	.global asm_putc	
/* expects character to be sent in r3.
   Destroys registers r16, r17 */
asm_putc:
	li	r16, UART0_BASE@l
	oris	r16, r16, UART0_BASE@h

	stb	r3, UART_THR(r16)

  /* check THRE bit, wait for transfer done */
wait:
	lbz	r17, UART_LSR(r16)
	andi.	r17, r17, 0x20
	beq	wait   // if result is zero. 

	blr


	.global asm_test_putc
asm_test_putc:
	mflr	r19

	// enable RTS, CTS
	li	r3, 0x0ef600304@l
	oris	r3, r3, 0x0ef600304@h
	li      r4, 3
	stb	r4, 0(r3)


	li	r3, 'x'
	bl	asm_putc


	li	r3, 'x'
	bl	asm_putc

	bl asm_beep
	li	r3, 'x'
	bl	asm_putc

	bl asm_beep
	li	r3, 'x'
	bl	asm_putc

	bl asm_beep
	li	r3, 'x'
	bl	asm_putc

	bl asm_beep
	li	r3, 'x'
	bl	asm_putc

	bl asm_beep
	li	r3, 'x'
	bl	asm_putc

	bl asm_beep
	li	r3, 'x'
	bl	asm_putc

	mtlr	r19
	blr


	.global asm_puts
 /* r3 points to zero terminated string to write. Destroys 
    registers r16-r19 */
asm_puts:
	mflr	r19
	mr	r18, r3
next:
	lbz	r3, 0(r18)
	cmpwi	r3, 0
	beq	end
	bl	asm_putc
	addi	r18, r18, 1
	b	next

end:
	mtlr	r19
	blr
	
	.global asm_getc

/* Polls for character and returns it in r3. Destroys register 16. */

as_getc:
	li	r16, UART0_BASE@l
	oris	r16, r16, UART0_BASE@h

poll_character:
	lbz	r3, UART_LSR(r16)

	andi.	r3, r3, asyncLSRDataReady1
	beq	poll_character // if bit is not set 
	

#if 0
	andi	r3, asyncLSRFramingError1|asyncLSROverrunError1|asyncLSRParityError1|asyncLSRBreakInterrupt1
	bne	error
#endif 
	

read:
	lbz	r3, UART_RBR(r16)
	blr


/* Just tests if a character is there .. destroys r16. 
   Returns nonzero if character is there. */
	.global asm_tstc
asm_tstc:
	li	r16, UART0_BASE@l
	oris	r16, r16, UART0_BASE@h

	lbz	r3, UART_LSR(r16)
	andi.	r3, r3, asyncLSRDataReady1

	blr

hex_digits:	
	.ascii	"0123456789abcdef"
	.align	4
x0:	
	.ascii "\r\n0x\0"
	.align	4

	.global	print_reg_hex
/* prints r3 as address of register. */

print_reg_hex:

	mflr	r27

	mr	r4, r3	// save adress

	bl	print_hex	//show adress

	li	r3, ' '
	bl	asm_putc
	li	r3, ':'
	bl	asm_putc
	li	r3, ' '
	bl	asm_putc

	lswi	r3, r4, 4	//read from address
	bl	print_hex

	li	r3, '\r'
	bl	asm_putc
	li	r3, '\n'
	bl	asm_putc

	mtlr	r27
	blr

	.global	print_hex
/* prints r3 as hex value .. destroys registers r16-r24, r3 is unchanged. */


print_hex:

	mflr	r23
	mr	r24, r3
	mr	r20, r3
	li	r22,58

	li	r3, ' '
	bl	asm_putc
	li	r3, '0'
	bl	asm_putc
	li	r3, 'x'	
	bl	asm_putc
	

	li	r21, 8
	mtctr	r21


next_digit:	
	srwi	r21, r20, 28	// places highest nibble of r20 in lowest
			        // of r21
	//lbzx	r3, r21, r22 

	addi	r3, r21, 48
	cmplw	r3, r22
	blt 	show

	addi	r3, r3, 7 


show:
	bl	asm_putc	

	slwi	r20, r20, 4
	bdnz	next_digit

	mr	r3, r24
	mtlr	r23
	blr
	

	.global print_some_regs
/* destroys registers r16-r26 */
	mr	r20, r3

#define PRINT_REG(n) \
	li	r3, ' '		; \
	bl	asm_putc		; \
	li	r3, 'r'		; \
	bl	asm_putc		; \
	li	r3, '0'+(n)	; \
	bl	asm_putc		; \
	li	r3, ':'		; \
	bl	asm_putc		; \
	mr	r3, (n)		; \
	bl	print_hex

print_some_regs:
	mflr	r26
	mr	r25, r3
	
/*	li	r3, '\r'	mr	r20, r3

	bl	putc */
	li	r3, '\n'
	bl	asm_putc

	PRINT_REG(5)
#if 0 	

	PRINT_REG(0)0xef600300

	PRINT_REG(25)   // really r3 
	PRINT_REG(4)
	PRINT_REG(5)
	PRINT_REG(8)	
#endif 
	mr	r20, r3

	mr	r3, r25
	mtlr	r26
	blr


	.global print_gpio
print_gpio:
	mflr	r26
	mr	r25, r3
	
	addis	r3, 0, 0xef60
	ori	r3, r3, 0x0700
	bl	print_reg_hex	


	addis	r3, 0, 0xef60
	ori	r3, r3, 0x0704
	bl	print_reg_hex

	addis	r3, 0, 0xef60
	ori	r3, r3, 0x0708
	bl	print_reg_hex

	addis	r3, 0, 0xef60
	ori	r3, r3, 0x070C
	bl	print_reg_hex

	addis	r3, 0, 0xef60
	ori	r3, r3, 0x0710
	bl	print_reg_hex

	addis	r3, 0, 0xef60
	ori	r3, r3, 0x0710
	bl	print_reg_hex

	addis	r3, 0, 0xef60
	ori	r3, r3, 0x0718
	bl	print_reg_hex


	mr	r3, r25
	mtlr	r26
	blr	


	.global print_tlb
print_tlb:
	mflr	r26
	mr	r25, r3


	addi	r1,r0,0x0000	    /* Tlb entry #0 */
	tlbre	r0,r1,0x0000		/* Read contents */
	li r3,r0
	bl print_hex
	tlbre	r0,r1,0x0001		/* Read contents */
	li r3,r0
	bl print_hex
	tlbre	r0,r1,0x0002		/* Read contents */
	li r3,r0
	bl print_hex

	mr	r3, r25
	mtlr	r26
	blr	


// int ReadTlb0(int Entry);
	.global ReadTlb0
ReadTlb0:
	tlbre	r3,r3,0x0000
	blr


// int ReadTlb1(int Entry);
	.global ReadTlb1
ReadTlb1:
	tlbre	r3,r3,0x0001
	blr

// int ReadTlb2(int Entry);
	.global ReadTlb2
ReadTlb2:
	tlbre	r3,r3,0x0002
	blr

