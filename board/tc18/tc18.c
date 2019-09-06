/*
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
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	 See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */

#include <common.h>
#include <ppc4xx.h>
#include <asm/processor.h>
#include <spd_sdram.h>
#include <command.h>
#include "lcd.h"


#ifndef CFG_NO_FLASH
extern flash_info_t flash_info[CFG_MAX_FLASH_BANKS]; /* info for FLASH chips	*/
#endif

extern void asm_beep(void);

extern void serial0_puts(const char *s);

static void get_status_reg(void) {


	char buffer[40];
	unsigned long val;

	val = mfspr(dbsr);
	sprintf(buffer,"\nDBSR:0x%08x\n",val);
	serial0_puts(buffer);	
	mtspr(dbsr,val);   //clear bits 
//	val = mfspr(dbsr); 
	val = mfspr(tsr);
	sprintf(buffer,"TSR :0x%08x\n",val);
	serial0_puts(buffer);	
	val=0xffff0000;
	mtspr(tsr,val);   //clear all timer exception status

}

#if 0

static void get_reg(char *regname,int reg_nr)
{
	register uint reg;
	char buffer[40];

	reg=in32(reg_nr);	
	sprintf(buffer,"%10s:0x%08x = 0x%08x\n",regname,reg_nr,reg);
	serial0_puts(buffer);	
}

void  print_gpio_init_f(void)
{
	get_reg("GPIO0_OR",GPIO0_OR);
	get_reg("GPIO0_TCR",GPIO0_TCR);
	get_reg("GPIO0_ODR",GPIO0_ODR);

	get_reg("GPIO0_OSRL",GPIO0_OSRL);
	get_reg("GPIO0_TSRL",GPIO0_TSRL);
	get_reg("GPIO0_ISR1L",GPIO0_ISR1L);
	get_reg("GPIO0_OSRH",GPIO0_OSRH);
	get_reg("GPIO0_TSRH",GPIO0_TSRH);
	get_reg("GPIO0_ISR1H",GPIO0_ISR1H);

	get_reg("GPIO1_OR",GPIO1_OR);
	get_reg("GPIO1_TCR",GPIO1_TCR);
	get_reg("GPIO1_ODR",GPIO1_ODR);

	get_reg("GPIO1_OSRL",GPIO1_OSRL);
	get_reg("GPIO1_TSRL",GPIO1_TSRL);
	get_reg("GPIO1_ISR1L",GPIO1_ISR1L);
	get_reg("GPIO1_OSRH",GPIO1_OSRH);
	get_reg("GPIO1_TSRH",GPIO1_TSRH);
	get_reg("GPIO1_ISR1H",GPIO1_ISR1H);

}
#endif


void  gpio_init_f(void)
{
	int i;

	/*--------------------------------------------------------------------
	 * Setup the GPIO pins
	 *-------------------------------------------------------------------*/
	// Set GPIOs to High-Z
	out32(GPIO0_TCR, 0);
	out32(GPIO0_TSRL, 0);
	out32(GPIO0_TSRH, 0);
	out32(GPIO0_ODR, 0);
	
	// Select output source
	out32(GPIO0_OR,    0x00000000);
	out32(GPIO0_OSRL,  0x00055455);
	out32(GPIO0_OSRH,  0x55555000);
	
	// Select correct ThreeState Control
	out32(GPIO0_TSRL,  0x00055455);
	out32(GPIO0_TSRH,  0x55555000);
	out32(GPIO0_TCR,   0x2010000f); //0x3c10f18f);

	// Select input Register Source	
	out32(GPIO0_ISR1L, 0x00000055);
	out32(GPIO0_ISR1H, 0x00541000);




	out32(GPIO1_TCR, 0xc0005fff);
	out32(GPIO1_ODR, 0x00000000);
	out32(GPIO1_OR,  0x00005401);

	out32(GPIO1_TSRL,  0x05555550);
	out32(GPIO1_OSRL,  0x05555550);
	out32(GPIO1_ISR1L, 0x04415555);

	out32(GPIO1_OSRH,  0x00000000);
	out32(GPIO1_TSRH,  0x00000000);
	out32(GPIO1_ISR1H, 0x40000000);

	//asm_beep();

/* remove resets*/
	for (i = 0; i < 1000000;) {i++;}

	out32(GPIO1_OR,  0x00005437);

}



/* bits in GPIO register 0 */
#define GPIO_JTAGNTRST	0x10000000

/* bits in GPIO register 1 */
#define GPIO_JTAG		0x00005800
#define GPIO_JTAG_TCK		0x00000800
#define GPIO_JTAG_TDI		0x00004000
#define GPIO_JTAG_TMS		0x00001000
#define GPIO_JTAGSEL		0x00000700
#define GPIO_JTAGSEL_PPC	0x00000000
#define GPIO_JTAGSEL_EXT	0x00000400

/*static void setbits (unsigned address, unsigned mask, unsigned value)
{
	unsigned temp;
	volatile unsigned * gpio = (unsigned *) address;
	
	temp = (* gpio) & ~mask | value;
	* gpio = temp;
}; */

#define SETBITS(address,mask,value)  out32(address, ((in32(address) & ~mask) | value) )



int  dsp_jtag_init(void)
{
	/*--------------------------------------------------------------------
	 * Generates TCK pulse with nTRST asserted
	 *-------------------------------------------------------------------*/
	SETBITS(GPIO0_ODR,GPIO_JTAGNTRST,GPIO_JTAGNTRST);
	/* assert nTRST */
	SETBITS(GPIO0_OR,GPIO_JTAGNTRST,0);
	//out32(GPIO0_OR,(in32(GPIO0_OR) & ~GPIO_JTAGNTRST | 0 ));
	/* get control of the JTAG chain */
	SETBITS(GPIO1_OR,GPIO_JTAGSEL,GPIO_JTAGSEL_PPC);
	/* generate pulse on TCK */
	SETBITS(GPIO1_OR,GPIO_JTAG,GPIO_JTAG_TMS|GPIO_JTAG_TDI);
	SETBITS(GPIO1_OR, GPIO_JTAG, GPIO_JTAG_TMS | GPIO_JTAG_TDI | GPIO_JTAG_TCK);
	SETBITS(GPIO1_OR, GPIO_JTAG, GPIO_JTAG_TMS | GPIO_JTAG_TDI);
	/* release nTRST */
	SETBITS(GPIO0_OR, GPIO_JTAGNTRST, GPIO_JTAGNTRST);
	/* generate pulse on TCK */
	SETBITS(GPIO1_OR, GPIO_JTAG, GPIO_JTAG_TMS | GPIO_JTAG_TDI | GPIO_JTAG_TCK);
	SETBITS(GPIO1_OR, GPIO_JTAG, GPIO_JTAG_TMS | GPIO_JTAG_TDI);
	/* set JTAG chain control back to external */
	SETBITS(GPIO1_OR, GPIO_JTAGSEL, GPIO_JTAGSEL_EXT);
	return 0;
}


void  gpio_init_for_beep(void)
{
	/*--------------------------------------------------------------------
	 * Setup the GPIO pins
	 *-------------------------------------------------------------------*/
	
	out32(GPIO0_OR,  in32(GPIO0_OR) | 0x00000000);
	out32(GPIO0_TCR,  in32(GPIO0_TCR) |0x00100000); //0x3c10f18f);
	out32(GPIO0_ODR,  in32(GPIO0_ODR) |0x00000000);

}



void signal_to_testpoint5(void)
{
	
	out32(GPIO0_OR, in32(GPIO0_OR) | 0x10000000);
	out32(GPIO0_OR, in32(GPIO0_OR) & ~0x10000000);
}

void beep(void)
{
	int i;
	
	out32(GPIO0_OR, in32(GPIO0_OR) | 0x100000);
	for (i = 0; i < 1000000;) {i++;}
	out32(GPIO0_OR, in32(GPIO0_OR) & ~0x100000);
	for (i = 0; i < 1000000;) {i++;}
}

void trace_msg(void)
{
	serial0_puts("test\n");
}


void read_sdram_conf(void);

int board_early_init_f(void)
{
	register uint reg;
 
//	serial0_puts("Start board_early_init_f\n");
        
get_status_reg();

 	/*--------------------------------------------------------------------
	 * Setup the external bus controller/chip selects
	 *-------------------------------------------------------------------*/
	mtdcr(ebccfga, xbcfg);
	reg = mfdcr(ebccfgd);
	mtdcr(ebccfgd, reg | 0x04000000);	/* Set ATC */


	/*--------------------------------------------------------------------
	 * Setup eeprom/flashgpio_init_f
	 *-------------------------------------------------------------------*/

#ifdef BOOT_EEPROM

       //---------------------------------------------------------------------
        // Memory Bank 0 (EEPROM at fff0 0000) 
        //---------------------------------------------------------------------
	mtebc(pb0ap, 0x03017200);	/* EEPROM */
	mtebc(pb0cr, 0xfff08000);	/* BAS=0xfff 1MB r 8-bit */


#else
       //---------------------------------------------------------------------
        // Memory Bank 0 (EEPROM at ff00 0000) 
        //---------------------------------------------------------------------
	mtebc(pb0ap, 0x03017300);	/* FLASH/SRAM */
	mtebc(pb0cr, 0xff09a000);	/* BAS=0xff0 16MB r/w 16-bit */
#endif



        //---------------------------------------------------------------------
        // Memory Bank 1 (Flash at 9000 0000) 
        //---------------------------------------------------------------------

	mtebc(pb1ap, 0x07855e00);	/* FLASH/SRAM */
	mtebc(pb1cr, 0x9009a000);	/* BAS=0x900 16MB r/w 16-bit */



#ifdef BOOT_EEPROM


       //---------------------------------------------------------------------
        // Memory Bank 2 (Flash at 9100 0000) 
        //---------------------------------------------------------------------

	mtebc(pb2ap, 0x07855e00);	/* FLASH/SRAM */
	mtebc(pb2cr, 0x9109a000);	/* BAS=0x910 16MB r/w 16-bit */


#endif

	/*--------------------------------------------------------------------
	 * Setup fpga
	 *-------------------------------------------------------------------*/
        //---------------------------------------------------------------------
        // Memory Bank 3 (FPGA/CF at 9200 0000) 
        //---------------------------------------------------------------------

	mtebc(pb3ap, 0x07855e00);	/* FPGA/CF */
	mtebc(pb3cr, 0x9209a000);	/* BAS=0x920 16MB r/w 16-bit */

       //---------------------------------------------------------------------
        // Memory Bank 4 (FPGA/CF in at 9300 0000) 
        //---------------------------------------------------------------------


	mtebc(pb4ap, 0x07855e00);	/* FPGA/CF */
	mtebc(pb4cr, 0x9309a000);	/* BAS=0x930 16MB r/w 16-bit */

	mtebc(pb5ap, 0x00000000);
	mtebc(pb5cr, 0x00000000);

	
	/*--------------------------------------------------------------------
	 * Setup the GPIO pins
	 *-------------------------------------------------------------------*/
	
	gpio_init_f();


	/*--------------------------------------------------------------------
	 * Setup the interrupt controller polarities, triggers, etc.
	 *-------------------------------------------------------------------*/
	mtdcr(uic0sr, 0xffffffff);	/* clear all */
	mtdcr(uic0er, 0x00000000);	/* disable all */
	mtdcr(uic0cr, 0x00000009);	/* ATI & UIC1 crit are critical */
	mtdcr(uic0pr, 0xfffffe13);	/* per ref-board manual */
	mtdcr(uic0tr, 0x01c00008);	/* per ref-board manual */
	mtdcr(uic0vr, 0x00000001);	/* int31 highest, base=0x000 */
	mtdcr(uic0sr, 0xffffffff);	/* clear all */



	mtdcr(uic1sr, 0xffffffff);	/* clear all */
	mtdcr(uic1er, 0x00000000);	/* disable all */
	mtdcr(uic1cr, 0x00000000);	/* all non-critical */
	mtdcr(uic1pr, 0xffffe0ff);	/* per ref-board manual */
	mtdcr(uic1tr, 0x00ffc000);	/* per ref-board manual */
	mtdcr(uic1vr, 0x00000001);	/* int31 highest, base=0x000 */
	mtdcr(uic1sr, 0xffffffff);	/* clear all */



	/*--------------------------------------------------------------------
	 * Setup other serial configuration
	 *-------------------------------------------------------------------*/
	mfsdr(sdr_pci0, reg);
	mtsdr(sdr_pci0, 0x80000000 | reg);	/* PCI arbiter enabled */
	mtsdr(sdr_pfc0, 0x00003e00);	/* Pin function */
	mtsdr(sdr_pfc1, 0x00048000);	/* Pin function: UART0 has 4 pins */


#if 0
	/*clear tmrclk divisor */
	*(unsigned char *)(CFG_BCSR_BASE | 0x04) = 0x00;

	/*enable ethernet */
	*(unsigned char *)(CFG_BCSR_BASE | 0x08) = 0xf0;

	/*get rid of flash write protect */
	*(unsigned char *)(CFG_BCSR_BASE | 0x07) = 0x00;
#endif 

//serial0_puts("End board_early_init_f\n");

	return 0;
}



int misc_init_r (void)
{
	DECLARE_GLOBAL_DATA_PTR;
	uint pbcr;
	int size_val = 0;

	/* Re-do sizing to get full correct info */
	mtdcr(ebccfga, pb0cr);
	pbcr = mfdcr(ebccfgd);
	switch (gd->bd->bi_flashsize) {
	case 1 << 20:
		size_val = 0;
		break;
	case 2 << 20:
		size_val = 1;
		break;
	case 4 << 20:
		size_val = 2;
		break;
	case 8 << 20:
		size_val = 3;
		break;
	case 16 << 20:
		size_val = 4;
		break;
	case 32 << 20:
		size_val = 5;
		break;
	case 64 << 20:
		size_val = 6;
		break;
	case 128 << 20:
		size_val = 7;
		break;
	}

	pbcr = (pbcr & 0x0001ffff) | gd->bd->bi_flashstart | (size_val << 17);
	mtdcr(ebccfga, pb0cr);
	mtdcr(ebccfgd, pbcr);

#ifndef CFG_NO_FLASH
	/* Monitor protection ON by default */
	(void)flash_protect(FLAG_PROTECT_SET,
			    -CFG_MONITOR_LEN,
			    0xffffffff,
			    &flash_info[0]);
#endif

	return 0;
}

int checkboard(void)
{
	unsigned long reg;
	sys_info_t sysinfo;
	unsigned char *s = getenv("serial#");

	get_sys_info(&sysinfo);

	printf("Board: TC18 - Topcall Line Server Board");
	if (s != NULL) {
		puts(", serial# ");
		puts(s);
	}
	putc('\n');

	printf("\tVCO: %lu MHz\n", sysinfo.freqVCOMhz / 1000000);
	printf("\tCPU: %lu MHz\n", sysinfo.freqProcessor / 1000000);
	printf("\tPLB: %lu MHz\n", sysinfo.freqPLB / 1000000);
	printf("\tFwdDivB: %d \n", sysinfo.pllFwdDivB);
	printf("\tOPB: %lu MHz\n", sysinfo.freqOPB / 1000000);
	printf("\tEPB: %lu MHz\n", sysinfo.freqEPB / 1000000);
	printf("\tPCI: %lu MHz\n", sysinfo.freqPCI / 1000000);

	mfclk(clk_plld, reg);

if (sysinfo.pllFwdDivB==3)
{
	reg = ((reg & ~PLLD_FWDVB_MASK) | (4<<8));
	//printf("new reg :%x\n",reg);
	mtclk(clk_plld, reg);

	mfclk(clk_icfg, reg);
	//printf("clk_icfg :%x\n",reg);
	reg = ((reg | 0x80000000));
	mtclk(clk_icfg, reg);

	//printf("chip reset\n");
	mtspr(dbcr0, 0x20000000); // chip reset

}




	return (0);
}

/*************************************************************************
 *  sdram_init -- doesn't use serial presence detect.
 *
 *  Assumes:    256 MB, ECC, non-registered
 *              PLB @ 133 MHz
 *
 ************************************************************************/
#define NUM_TRIES 64
#define NUM_READS 10
#define MIN_GOOD_RANGE 50
void sdram_tr1_set(int ram_address, int* tr1_value)
{
	int i;
	int j, k;
	volatile unsigned int* ram_pointer =  (unsigned int*)ram_address;
	int first_good = -1, last_bad = 0x1ff;
	int HighOk, LowOk;
	int Lastj =-1;
	char buffer[80];
		

	unsigned long test[NUM_TRIES] = {
		       	0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
			0x00000000, 0x00000000, 0xFFFFFFFF, 0xFFFFFFFF,
			0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000,
			0xFFFFFFFF, 0xFFFFFFFF, 0x00000000, 0x00000000,
			0xAAAAAAAA, 0xAAAAAAAA, 0x55555555, 0x55555555,
			0xAAAAAAAA, 0xAAAAAAAA, 0x55555555, 0x55555555,
			0x55555555, 0x55555555, 0xAAAAAAAA, 0xAAAAAAAA,
			0x55555555, 0x55555555, 0xAAAAAAAA, 0xAAAAAAAA,
			0xA5A5A5A5, 0xA5A5A5A5, 0x5A5A5A5A, 0x5A5A5A5A,
			0xA5A5A5A5, 0xA5A5A5A5, 0x5A5A5A5A, 0x5A5A5A5A,
			0x5A5A5A5A, 0x5A5A5A5A, 0xA5A5A5A5, 0xA5A5A5A5,
			0x5A5A5A5A, 0x5A5A5A5A, 0xA5A5A5A5, 0xA5A5A5A5,
			0xAA55AA55, 0xAA55AA55, 0x55AA55AA, 0x55AA55AA,
			0xAA55AA55, 0xAA55AA55, 0x55AA55AA, 0x55AA55AA,
			0x55AA55AA, 0x55AA55AA, 0xAA55AA55, 0xAA55AA55,
			0x55AA55AA, 0x55AA55AA, 0xAA55AA55, 0xAA55AA55 };
	
	//go through all possible SDRAM0_TR1[RDCT] values
	for(i=0; i<=0x1ff; i++)

	{
		//set the current value for TR1
		mtsdram(mem_tr1, (0x80800000 | i) );
		
		//write values
		for(j=0; j<NUM_TRIES; j++)
		{
			ram_pointer[j] = test[j];	
			
			//clear any cache at ram location
			__asm__("dcbf 0,%0": :"r" (&ram_pointer[j]));
		}
		
		//read values back
		for(j=0; j<NUM_TRIES; j++)
		{
			HighOk = 0; 
			LowOk = 0;
			for(k=0; k<NUM_READS; k++)
			{
				//clear any cache at ram location
				__asm__("dcbf 0,%0": :"r" (&ram_pointer[j]));
		//		printf("  RAM=%8x Test=%8x\n", ram_pointer[j], test[j]);
				if (((ram_pointer[j] ^ test[j]) & 0xffffffff) == 0)
					LowOk++;
				if (((ram_pointer[j] ^ test[j]) & 0xffffffff) == 0) {
					HighOk++;
				}
				else {
					//printf(" RAM=%8x Test=%8x\n", ram_pointer[j], test[j]);
				}
				//if(ram_pointer[j] != test[j]) 
				//	break;	
			}
			
		//	printf("DDR ConfigValue = %i, Try=%i, HiOk=%i, LowOk=%i\n", i, j, HighOk,LowOk);
			//read error
			if (HighOk != NUM_READS)
			{
				break;
			}
		}
		
		if (j != Lastj) {
//			printf("ConfigValue = %i, NumberOfGoodReads=%i\n", i, j);
			Lastj = j;
		}
	
		//we have a SDRAM0_TR1[RDCT] that is part of the window
		if(j==NUM_TRIES)
		{
			if(first_good == -1)
				first_good = i;		//found beginning of window
		}else //bad read
		{
			//if we have not had a good read then don't care
			if(first_good != -1) 
			{
				//first failure after a good read
				if((i - first_good) >= MIN_GOOD_RANGE)
				{
					//first failure after a good read
					last_bad = i-1;
					break;
				}
				else {
					//if we have not had a good read then don't care
					sprintf(buffer,"DDR Config: Good delay values from %i to %i ignored because the range is too small\n",first_good, i);
					serial0_puts(buffer);	
					first_good = -1;
				}
				
			}
		}
	}
	
	//return the current value for TR1
	*tr1_value = (first_good + last_bad)/2;
	//printf("DDR ConfigValue first_good = %i, last_bad=%i, HiOk=%i, LowOk=%i\n",first_good, last_bad , HighOk,LowOk);
	if((last_bad - first_good) < MIN_GOOD_RANGE) {
		lcd_print (16,  "STOP - RAM error");
		for(;;);
	}
	else {
		sprintf(buffer,"DDR Config: Good delay values found from %i to %i\n",first_good, last_bad );
		serial0_puts(buffer);	
	}

}

void sdram_init(void)
{
	register uint reg;
	int tr1_bank1;

	/*--------------------------------------------------------------------
	 * Setup some default
	 *------------------------------------------------------------------*/
	mtsdram(mem_uabba, 0x00000000);	/* ubba=0 (default)             */
	mtsdram(mem_slio, 0x00000000);	/* rdre=0 wrre=0 rarw=0         */
	mtsdram(mem_devopt, 0x00000000);	/* dll=0 ds=0 (normal)          */
	mtsdram(mem_clktr, 0x40000000);	/* ?? */
	mtsdram(mem_wddctr, 0x40000000);	/* ?? */

	/*clear this first, if the DDR is enabled by a debugger
	   then you can not make changes. */
	mtsdram(mem_cfg0, 0x00000000);	/* Disable EEC */

	/*--------------------------------------------------------------------
	 * Setup for board-specific specific mem
	 *------------------------------------------------------------------*/
	/*
	 * Following for CAS Latency = 2.5 @ 133 MHz PLB
	 */
	mtsdram(mem_b0cr, 0x000a4001);	/* SDBA=0x000 128MB, Mode 3, enabled */
	
	mtsdram(mem_tr0, 0x410a4012);	/* ?? */
	//mtsdram(mem_tr1, 0x8080080b);	/* ?? */
	mtsdram(mem_tr1, 0x80800000 );

	mtsdram(mem_rtr, 0x04080000);	/* ?? */
	mtsdram(mem_cfg1, 0x00000000);	/* Self-refresh exit, disable PM    */
	mtsdram(mem_cfg0, 0x34000000);	/* Disable EEC */
	udelay(400);		/* Delay 200 usecs (min)            */

	/*--------------------------------------------------------------------
	 * Enable the controller, then wait for DCEN to complete
	 *------------------------------------------------------------------*/
	mtsdram(mem_cfg0, 0x84000000);	/* Enable */

	for (;;) {
		mfsdram(mem_mcsts, reg);
		if (reg & 0x80000000)
			break;
	}
	
	sdram_tr1_set(0x00000000, &tr1_bank1);   
	mtsdram(mem_tr1, (tr1_bank1 | 0x80800000) );



}
void get_sdram(char *regname,int reg_nr)
{
	register uint reg;
	char buffer[40];

	mfsdram(reg_nr, reg);	
	sprintf(buffer,"%10s:0x%08x = 0x%08x\n",regname,reg_nr,reg);
	serial0_puts(buffer);	
}


void read_sdram_conf(void)
{

	/*--------------------------------------------------------------------
	 * Setup some default
	 *------------------------------------------------------------------*/
	get_sdram("uabba",mem_uabba);	/* ubba=0 (default)             */
	get_sdram("slio",mem_slio);	/* rdre=0 wrre=0 rarw=0         */
	get_sdram("devopt",mem_devopt);	/* dll=0 ds=0 (normal)          */
	get_sdram("cltr",mem_clktr);	/* ?? */
	get_sdram("wddctr",mem_wddctr);	/* ?? */

	get_sdram("bc0r",mem_b0cr);	/* SDBA=0x000 128MB, Mode 3, enabled */
	get_sdram("bc1r",mem_b1cr);	/* SDBA=0x000 128MB, Mode 3, enabled */
	
	get_sdram("tr0",mem_tr0);	/* ?? */
	get_sdram("tr1",mem_tr1);
	get_sdram("rtr",mem_rtr);	/* ?? */
	get_sdram("cfg1",mem_cfg1);	/* Self-refresh exit, disable PM    */
	get_sdram("cfg0",mem_cfg0);	/* Disable EEC */
}


void get_ebc(char *regname,int reg_nr)
{
	register uint reg;
	char buffer[40];

	mfebc(reg_nr, reg);	

	sprintf(buffer,"%10s:0x%08x = 0x%08x\n",regname,reg_nr,reg);
	serial0_puts(buffer);	

}

int read_ebc_conf(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	get_ebc("pb0ap",pb0ap);	
	get_ebc("pb0cr",pb0cr);	
	get_ebc("pb1ap",pb1ap);	
	get_ebc("pb1cr",pb1cr);	
	get_ebc("pb2ap",pb2ap);	
	get_ebc("pb2cr",pb2cr);	
	get_ebc("pb3ap",pb3ap);	
	get_ebc("pb3cr",pb3cr);	
	return 0;

}


U_BOOT_CMD( \
	readebc, CFG_MAXARGS, 0, read_ebc_conf, \
	"readebc    - read ebc.\n", \
	"readebc    - read ebc.\n" \
);


/*************************************************************************
 *  long int initdram
 *
 ************************************************************************/
long int initdram(int board)
{
#ifndef CFG_BOOT_RAM_TEST
	sdram_init();
#endif
//read_sdram_conf();
	return CFG_SDRAM_BANKS * (CFG_KBYTES_SDRAM * 1024);	/* return bytes */
}

#if defined(CFG_DRAM_TEST)

#if 0

int hr_testdram(void)
{
	unsigned long *mem = (unsigned long *)0;
//	const unsigned long kend = (1024 / sizeof(unsigned long));
	unsigned long k;//, n;

	printf("SDRAM test start\n");


	mtmsr(0);
	#ifdef CFG_BOOT_RAM_TEST
	printf("SDRAM test ignore\n");
	return 0;
	#endif
	for (k=0; k<100; k++) {
		unsigned  i1[0], i2[0];
		memset(mem+0x000, 0xaa, 1024);
		memset(mem+0x124, 0xff, 1024);
		memset(mem+0x234, 0xaa, 1024);
		i1[0] = (mem[0x000] >> 16);
		i1[1] = (mem[0x124] >> 16);
		i1[2] = (mem[0x234] >> 16);
		memset(mem+0x000, 0x55, 1024);
		memset(mem+0x124, 0xaa, 1024);
		memset(mem+0x234, 0x55, 1024);
		i2[0] = (mem[0x000] >> 16);
		i2[1] = (mem[0x124] >> 16);
		i2[2] = (mem[0x234] >> 16);
		printf("%d. %4x/%4x %4x/%4x %4x/%4x\n",k,
			i2[0], i1[0], 
			i1[1], i2[1], 
			i2[2], i1[2]);
	}
#if 0
	for (k = 0; k < 1024 /*CFG_KBYTES_SDRAM*/;
	     ++k, mem += (1024 / sizeof(unsigned long))) {
		printf("k:%d  mem:%x\n",k,mem );
		if ((k & 1023) == 0) {
			printf("%3d MB\r", k / 1024);
		}

		memset(mem, 0xaaaaaaaa, 1024);
		for (n = 0; n < kend; ++n) {
			if (mem[n] != 0xaaaaaaaa) {
				printf("SDRAM test 0xaa fails at: %08x ReadValue=%08x\n",
				       (uint) & mem[n], mem[n]);
				//return 1;
			}
		ext. mem}	


		memset(mem, 0x55555555, 1024);
		for (n = 0; n < kend; ++n) {
			if (mem[n] != 0x55555555) {
				printf("SDRAM test 0x55 fails at: %08x ReadValue=%08x\n",
				       (uint) & mem[n], mem[n]);
				//return 1;
			}
		}
	}

#endif


printf("xxxxxxxxxxxx Test with printf\n");

	unsigned long i_r[4];
	i_r[0] = 0x55555555;
	i_r[1] = 0xAAAAAAAA;
	i_r[2] = 0x55555555;
	i_r[3] = 0xAAAAAAAA;

	for (k=0; k<100; k++) {
		unsigned i[4];
signal_to_testpoint5();

		*((volatile unsigned *)0x00555550) = i_r[0];
		*((volatile unsigned *)0x00555554) = i_r[1];
		*((volatile unsigned *)0x00555558) = i_r[2];
		*((volatile unsigned *)0x0055555C) = i_r[3];

		i[0] = *((volatile unsigned *)0x00555550);
		i[1] = *((volatile unsigned *)0x00555554);
		i[2] = *((volatile unsigned *)0x00555558);
		i[3] = *((volatile unsigned *)0x0055555c);
		printf("%4d.  %8x %8x %8x %8x\n", k, i[0], i[1], i[2], i[3]);
	}
	printf("Ref.  %8x %8x %8x %8x\n", i_r[0], i_r[1], i_r[2], i_r[3]);

printf("yyyyyyyyyyyyy Test endless without printf\n");




	for (;;) {
		unsigned i[4];

signal_to_testpoint5();

		*((volatile unsigned *)0x00555550) = 0x55555555;
		*((volatile unsigned *)0x00555554) = 0xAAAAAAAA;
		*((volatile unsigned *)0x00555558) = 0x55555555;
		*((volatile unsigned *)0x0055555C) = 0xAAAAAAAA;

		i[0] = *((volatile unsigned *)0x00555550);
		i[1] = *((volatile unsigned *)0x00555554);
		i[2] = *((volatile unsigned *)0x00555558);
		i[3] = *((volatile unsigned *)0x0055555c);
	}



}

#endif

int testdram_mask(unsigned long mask)
{
	unsigned long *mem = (unsigned long *)0;
	const unsigned long kend = (1024 / sizeof(unsigned long));
	unsigned long k, n;
	unsigned long okaa, nokaa;
	unsigned long ok55, nok55;
	unsigned long ok12, nok12;
	




	okaa=0;
	nokaa=0;
	ok55=0;
	nok55=0;
	ok12=0;
	nok12=0;

	mtmsr(0);

//valtest=0x12345678;


	for (k = 0; k < CFG_KBYTES_SDRAM;
	     ++k, mem += (1024 / sizeof(unsigned long))) {

		if ((k & 1023) == 0) {
			printf("%3d MB\r", k / 1024);
		}

		//memset(mem, 0x12345678, 1024);
		for (n = 0; n < kend; ++n)
			mem[n] = (unsigned long )&(mem[n]);
		for (n = 0; n < kend; ++n) {
			if (((mem[n] ^ (unsigned long )(&(mem[n]))) & mask) == 0) {
				ok55++;
			}
			else {
				//printf("0x12345678  %8x\n", mem[n]);
				nok55++;
			}
		}

		for (n = 0; n < kend; ++n)
			mem[n] = 0xffffffff-(unsigned long )(&(mem[n]));
		for (n = 0; n < kend; ++n) {
			if (((mem[n] ^ (0xffffffff-(unsigned long )&(mem[n]))) & mask) == 0) {
				ok12++;
			}
			else {
				nok12++;
			}
		}


#if 0
		memset(mem, 0xaaaaaaaa, 1024);
		for (n = 0; n < kend; ++n) {
			if (((mem[n] ^ 0xaaaaaaaa) & mask) == 0) {
				okaa++;
			}
			else {
				nokaa++;
			}

		}	
		

		memset(mem, 0x55555555, 1024);
		for (n = 0; n < kend; ++n) {
			if (((mem[n] ^ 0x55555555) & mask) == 0) {
				ok55++;
			}
			else {
				nok55++;
			}
		}	

#endif

	}

	//printf("DRAM Test(ok/nok) Mask:%08x aaaaaaaa: %d / %d 55555555: %d/%d 12345678: %d/%d\n",mask,okaa,nokaa,ok55,nok55,ok12,nok12);

	printf("DRAM Test(ok/nok) Mask:%08x &mem[n]: %d/%d ffffff-&mem[n]: %d/%d\n",mask,ok55,nok55,ok12,nok12);
	return 0;
}

int ext_testdram(cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
	testdram_mask(0xff000000);
	testdram_mask(0x00ff0000);
	testdram_mask(0x0000ff00);
	testdram_mask(0x000000ff);
	return 0;
}

U_BOOT_CMD(
	extmtest,	CFG_MAXARGS,	0,	ext_testdram,
	"extmtest    - ext. mem.\n",""\
);


int testdram(void)
{
	unsigned long *mem = (unsigned long *)0;
	const unsigned long kend = (1024 / sizeof(unsigned long));
	unsigned long k, n;
	char tmp[64];	/* long enough for environment variables */
	int i = getenv_r ("testdram", tmp, sizeof (tmp));


	if ((i == 0) || ((int) simple_strtoul (tmp, NULL, 10) == 0 ) ){
		printf("SDRAM test skipped!\n");
		mtmsr(0);
		for (k = 0; k < CFG_KBYTES_SDRAM;
		++k, mem += (1024 / sizeof(unsigned long))) {
			if ((k & 1023) == 0) {
				printf("%3d MB\r", k / 1024);
			}
			memset(mem, 0x0, 1024);
		}
	}
	else {

		mtmsr(0);
	
		for (k = 0; k < CFG_KBYTES_SDRAM;
		++k, mem += (1024 / sizeof(unsigned long))) {
			if ((k & 1023) == 0) {
				printf("%3d MB\r", k / 1024);
			}
	
			memset(mem, 0xaaaaaaaa, 1024);
			for (n = 0; n < kend; ++n) {
				if (mem[n] != 0xaaaaaaaa) {
					printf("SDRAM test fails at: %08x\n",
					(uint) & mem[n]);
					return 1;
				}
			}
	
			memset(mem, 0x55555555, 1024);
			for (n = 0; n < kend; ++n) {
				if (mem[n] != 0x55555555) {
					printf("SDRAM test fails at: %08x\n",
					(uint) & mem[n]);
					return 1;
				}
			}
		}
		printf("SDRAM test passes\n");
	}
	return 0;
}

#endif

/*************************************************************************
 *  pci_pre_init
 *
 *  This routine is called just prior to registering the hose and gives
 *  the board the opportunity to check things. Returning a value of zero
 *  indicates that things are bad & PCI initialization should be aborted.
 *
 *	Different boards may wish to customize the pci controller structure
 *	(add regions, override default access routines, etc) or perform
 *	certain pre-initialization actions.
 *
 ************************************************************************/
#if defined(CONFIG_PCI) && defined(CFG_PCI_PRE_INIT)
int pci_pre_init(struct pci_controller *hose)
{
	unsigned long strap;
	unsigned long addr;

	/*--------------------------------------------------------------------------+
	 *	Bamboo is always configured as the host & requires the
	 *	PCI arbiter to be enabled.
	 *--------------------------------------------------------------------------*/
	mfsdr(sdr_sdstp1, strap);
	if ((strap & SDR0_SDSTP1_PAE_MASK) == 0) {
		printf("PCI: SDR0_STRP1[PAE] not set.\n");
		printf("PCI: Configuration aborted.\n");
		return 0;
	}

	/*-------------------------------------------------------------------------+
	  | Set priority for all PLB3 devices to 0.
	  | Set PLB3 arbiter to fair mode.
	  +-------------------------------------------------------------------------*/
	mfsdr(sdr_amp1, addr);
	mtsdr(sdr_amp1, (addr & 0x000000FF) | 0x0000FF00);
	addr = mfdcr(plb3_acr);
	mtdcr(plb3_acr, addr | 0x80000000);

	/*-------------------------------------------------------------------------+
	  | Set priority for all PLB4 devices to 0.
	  +-------------------------------------------------------------------------*/
	mfsdr(sdr_amp0, addr);
	mtsdr(sdr_amp0, (addr & 0x000000FF) | 0x0000FF00);
	addr = mfdcr(plb4_acr) | 0xa0000000;	/* Was 0x8---- */
	mtdcr(plb4_acr, addr);

	/*-------------------------------------------------------------------------+
	  | Set Nebula PLB4 arbiter to fair mode.
	  +-------------------------------------------------------------------------*/
	/* Segment0 */
	addr = (mfdcr(plb0_acr) & ~plb0_acr_ppm_mask) | plb0_acr_ppm_fair;
	addr = (addr & ~plb0_acr_hbu_mask) | plb0_acr_hbu_enabled;
	addr = (addr & ~plb0_acr_rdp_mask) | plb0_acr_rdp_4deep;
	addr = (addr & ~plb0_acr_wrp_mask) | plb0_acr_wrp_2deep;
	mtdcr(plb0_acr, addr);

	/* Segment1 */
	addr = (mfdcr(plb1_acr) & ~plb1_acr_ppm_mask) | plb1_acr_ppm_fair;
	addr = (addr & ~plb1_acr_hbu_mask) | plb1_acr_hbu_enabled;
	addr = (addr & ~plb1_acr_rdp_mask) | plb1_acr_rdp_4deep;
	addr = (addr & ~plb1_acr_wrp_mask) | plb1_acr_wrp_2deep;
	mtdcr(plb1_acr, addr);

	return 1;
}
#endif				/* defined(CONFIG_PCI) && defined(CFG_PCI_PRE_INIT) */

/*************************************************************************
 *  pci_target_init
 *
 *	The bootstrap configuration provides default settings for the pci
 *	inbound map (PIM). But the bootstrap config choices are limited and
 *	may not be sufficient for a given board.
 *
 ************************************************************************/
#if defined(CONFIG_PCI) && defined(CFG_PCI_TARGET_INIT)
void pci_target_init(struct pci_controller *hose)
{
	/*--------------------------------------------------------------------------+
	 * Set up Direct MMIO registers
	 *--------------------------------------------------------------------------*/
	/*--------------------------------------------------------------------------+
	  | PowerPC440 EP PCI Master configuration.
	  | Map one 1Gig range of PLB/processor addresses to PCI memory space.
	  |   PLB address 0xA0000000-0xDFFFFFFF ==> PCI address 0xA0000000-0xDFFFFFFF
	  |   Use byte reversed out routines to handle endianess.
	  | Make this region non-prefetchable.
	  +--------------------------------------------------------------------------*/
	out32r(PCIX0_PMM0MA, 0x00000000);	/* PMM0 Mask/Attribute - disabled b4 setting */
	out32r(PCIX0_PMM0LA, CFG_PCI_MEMBASE);	/* PMM0 Local Address */
	out32r(PCIX0_PMM0PCILA, CFG_PCI_MEMBASE);	/* PMM0 PCI Low Address */
	out32r(PCIX0_PMM0PCIHA, 0x00000000);	/* PMM0 PCI High Address */
	out32r(PCIX0_PMM0MA, 0xE0000001);	/* 512M + No prefetching, and enable region */

	out32r(PCIX0_PMM1MA, 0x00000000);	/* PMM0 Mask/Attribute - disabled b4 setting */
	out32r(PCIX0_PMM1LA, CFG_PCI_MEMBASE2);	/* PMM0 Local Address */
	out32r(PCIX0_PMM1PCILA, CFG_PCI_MEMBASE2);	/* PMM0 PCI Low Address */
	out32r(PCIX0_PMM1PCIHA, 0x00000000);	/* PMM0 PCI High Address */
	out32r(PCIX0_PMM1MA, 0xE0000001);	/* 512M + No prefetching, and enable region */

	out32r(PCIX0_PTM1MS, 0x00000001);	/* Memory Size/Attribute */
	out32r(PCIX0_PTM1LA, 0);	/* Local Addr. Reg */
	out32r(PCIX0_PTM2MS, 0);	/* Memory Size/Attribute */
	out32r(PCIX0_PTM2LA, 0);	/* Local Addr. Reg */

	/*--------------------------------------------------------------------------+
	 * Set up Configuration registers
	 *--------------------------------------------------------------------------*/

	/* Program the board's subsystem id/vendor id */
	pci_write_config_word(0, PCI_SUBSYSTEM_VENDOR_ID,
			      CFG_PCI_SUBSYS_VENDORID);
	pci_write_config_word(0, PCI_SUBSYSTEM_ID, CFG_PCI_SUBSYS_ID);

	/* Configure command register as bus master */
	pci_write_config_word(0, PCI_COMMAND, PCI_COMMAND_MASTER);

	/* 240nS PCI clock */
	pci_write_config_word(0, PCI_LATENCY_TIMER, 1);

	/* No error reporting */
	pci_write_config_word(0, PCI_ERREN, 0);

	pci_write_config_dword(0, PCI_BRDGOPT2, 0x00000101);

}
#endif				/* defined(CONFIG_PCI) && defined(CFG_PCI_TARGET_INIT) */

/*************************************************************************
 *  pci_master_init
 *
 ************************************************************************/
#if defined(CONFIG_PCI) && defined(CFG_PCI_MASTER_INIT)
void pci_master_init(struct pci_controller *hose)
{
	unsigned short temp_short;

	/*--------------------------------------------------------------------------+
	  | Write the PowerPC440 EP PCI Configuration regs.
	  |   Enable PowerPC440 EP to be a master on the PCI bus (PMM).
	  |   Enable PowerPC440 EP to act as a PCI memory target (PTM).
	  +--------------------------------------------------------------------------*/
	pci_read_config_word(0, PCI_COMMAND, &temp_short);
	pci_write_config_word(0, PCI_COMMAND,
			      temp_short | PCI_COMMAND_MASTER |
			      PCI_COMMAND_MEMORY);
}
#endif				/* defined(CONFIG_PCI) && defined(CFG_PCI_MASTER_INIT) */

/*************************************************************************
 *  is_pci_host
 *
 *	This routine is called to determine if a pci scan should be
 *	performed. With various hardware environments (especially cPCI and
 *	PPMC) it's insufficient to depend on the state of the arbiter enable
 *	bit in the strap register, or generic host/adapter assumptions.
 *
 *	Rather than hard-code a bad assumption in the general 440 code, the
 *	440 pci code requires the board to decide at runtime.
 *
 *	Return 0 for adapter mode, non-zero for host (monarch) mode.
 *
 *
 ************************************************************************/
#if defined(CONFIG_PCI)
int is_pci_host(struct pci_controller *hose)
{
	/* Bamboo is always configured as host. */
	return (1);
}

#endif				/* defined(CONFIG_PCI) */

/*************************************************************************
 *  hw_watchdog_reset
 *
 *	This routine is called to reset (keep alive) the watchdog timer
 *
 ************************************************************************/
#if defined(CONFIG_HW_WATCHDOG)
void hw_watchdog_reset(void)
{

}
#endif

void reset_phy (void)
  /* Old method (before EPLD actually worked) was to use the 
     GPIO's (there was a on-board workaround i.e. a wire), like
     this:  
	*gpio_or = 0x202; 

     Note that un-resetting Flash and PHY via EPLD requires that 
     EPLD is programmed (using the jtag command) */

{
/* Old method (before EPLD actually worked) was to use the 
     GPIO's (there was a on-board workaround i.e. a wire), like
     this:  
	*gpio_or*/

	out32(GPIO1_OR,  in32(GPIO1_OR) & ~0x00000030);

}

