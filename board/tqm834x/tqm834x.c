/*
 * (C) Copyright 2005
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
 *
 */

#include <common.h>
#include <ioports.h>
#include <mpc83xx.h>
#include <asm/mpc8349_pci.h>
#include <i2c.h>
#include <spd.h>
#include <miiphy.h>
#include <asm-ppc/mmu.h>
#include <pci.h>

#define IOSYNC			asm("eieio")
#define ISYNC			asm("isync")
#define SYNC			asm("sync")
#define FPW			FLASH_PORT_WIDTH
#define FPWV			FLASH_PORT_WIDTHV

#define DDR_MAX_SIZE_PER_CS	0x20000000

#if defined(DDR_CASLAT_20)
#define TIMING_CASLAT		TIMING_CFG1_CASLAT_20
#define MODE_CASLAT		DDR_MODE_CASLAT_20
#else
#define TIMING_CASLAT		TIMING_CFG1_CASLAT_25
#define MODE_CASLAT		DDR_MODE_CASLAT_25
#endif

#define INITIAL_CS_CONFIG	(CSCONFIG_EN | CSCONFIG_ROW_BIT_12 | \
				CSCONFIG_COL_BIT_9)

/* Global variable used to store detected number of banks */
int tqm834x_num_flash_banks;

/* External definitions */
ulong flash_get_size (ulong base, int banknum);
extern flash_info_t flash_info[];
extern long spd_sdram (void);

/* Local functions */
static int detect_num_flash_banks(void);
static long int get_ddr_bank_size(short cs, volatile long *base);
static void set_cs_bounds(short cs, long base, long size);
static void set_cs_config(short cs, long config);
static void set_ddr_config(void);

/* Local variable */
static volatile immap_t *im = (immap_t *)CFG_IMMRBAR;

/**************************************************************************
 * Board initialzation after relocation to RAM. Used to detect the number
 * of Flash banks on TQM834x.
 */
int board_early_init_r (void) {
	/* sanity check, IMMARBAR should be mirrored at offset zero of IMMR */
	if ((im->sysconf.immrbar & IMMRBAR_BASE_ADDR) != (u32)im)
		return 0;
	
	/* detect the number of Flash banks */
	return detect_num_flash_banks();
}

/**************************************************************************
 * DRAM initalization and size detection
 */
long int initdram (int board_type)
{
	long bank_size;
	long size;
	int cs;

	/* during size detection, set up the max DDRLAW size */
	im->sysconf.ddrlaw[0].bar = CFG_DDR_BASE;
	im->sysconf.ddrlaw[0].ar = (LAWAR_EN | LAWAR_SIZE_2G);

	/* set CS bounds to maximum size */
	for(cs = 0; cs < 4; ++cs) {
		set_cs_bounds(cs,
			CFG_DDR_BASE + (cs * DDR_MAX_SIZE_PER_CS),
			DDR_MAX_SIZE_PER_CS);

		set_cs_config(cs, INITIAL_CS_CONFIG);
	}

	/* configure ddr controller */
	set_ddr_config();

	udelay(200);
	
	/* enable DDR controller */
	im->ddr.sdram_cfg = (SDRAM_CFG_MEM_EN |
		SDRAM_CFG_SREN |
		SDRAM_CFG_SDRAM_TYPE_DDR);
	SYNC;

	/* size detection */
	debug("\n");
	size = 0;
	for(cs = 0; cs < 4; ++cs) {
		debug("\nDetecting Bank%d\n", cs);

		bank_size = get_ddr_bank_size(cs,
			(volatile long*)(CFG_DDR_BASE + size));
		size += bank_size;

		debug("DDR Bank%d size: %d MiB\n\n", cs, bank_size >> 20);

		/* exit if less than one bank */
		if(size < DDR_MAX_SIZE_PER_CS) break;
	}

	return size;
}

/**************************************************************************
 * checkboard()
 */
int checkboard (void)
{
	puts("Board: TQM834x\n");

#ifdef CONFIG_PCI
	DECLARE_GLOBAL_DATA_PTR;
	volatile immap_t * immr;
	u32 w, f;

	immr = (immap_t *)CFG_IMMRBAR;
	if (!(immr->reset.rcwh & RCWH_PCIHOST)) {
		printf("PCI:   NOT in host mode..?!\n");
		return 0;
	}

	/* get bus width */
	w = 32;
	if (immr->reset.rcwh & RCWH_PCI64) 
		w = 64;

	/* get clock */
	f = gd->pci_clk;

	printf("PCI1:  %d bit, %d MHz\n", w, f / 1000000);
#else
	printf("PCI:   disabled\n");
#endif
	return 0;
}


/**************************************************************************
 *
 * Local functions
 *
 *************************************************************************/

/**************************************************************************
 * Detect the number of flash banks (1 or 2). Store it in
 * a global variable tqm834x_num_flash_banks.
 * Bank detection code based on the Monitor code.
 */
static int detect_num_flash_banks(void)
{
	typedef unsigned long FLASH_PORT_WIDTH;
	typedef volatile unsigned long FLASH_PORT_WIDTHV;
	FPWV *bank1_base;
	FPWV *bank2_base;
	FPW bank1_read;
	FPW bank2_read;
	ulong bank1_size;
	ulong bank2_size;
	ulong total_size;

	tqm834x_num_flash_banks = 2;	/* assume two banks */
	
	/* Get bank 1 and 2 information */
	bank1_size = flash_get_size(CFG_FLASH_BASE, 0);
	debug("Bank1 size: %lu\n", bank1_size);
	bank2_size = flash_get_size(CFG_FLASH_BASE + bank1_size, 1);
	debug("Bank2 size: %lu\n", bank2_size);
	total_size = bank1_size + bank2_size;

	if (bank2_size > 0) {
		/* Seems like we've got bank 2, but maybe it's mirrored 1 */

		/* Set the base addresses */
		bank1_base = (FPWV *) (CFG_FLASH_BASE);
		bank2_base = (FPWV *) (CFG_FLASH_BASE + bank1_size);

		/* Put bank 2 into CFI command mode and read */
		bank2_base[0x55] = 0x00980098;
		IOSYNC;
		ISYNC;
		bank2_read = bank2_base[0x10];

		/* Read from bank 1 (it's in read mode) */
		bank1_read = bank1_base[0x10];

		/* Reset Flash */
		bank1_base[0] = 0x00F000F0;
		bank2_base[0] = 0x00F000F0;

		if (bank2_read == bank1_read) {
			/*
			 * Looks like just one bank, but not sure yet. Let's
			 * read from bank 2 in autosoelect mode.
			 */
			bank2_base[0x0555] = 0x00AA00AA;
			bank2_base[0x02AA] = 0x00550055;
			bank2_base[0x0555] = 0x00900090;
			IOSYNC;
			ISYNC;
			bank2_read = bank2_base[0x10];

			/* Read from bank 1 (it's in read mode) */
			bank1_read = bank1_base[0x10];

			/* Reset Flash */
			bank1_base[0] = 0x00F000F0;
			bank2_base[0] = 0x00F000F0;

			if (bank2_read == bank1_read) {
				/*
				 * In both CFI command and autoselect modes,
				 * we got the some data reading from Flash.
				 * There is only one mirrored bank.
				 */
				tqm834x_num_flash_banks = 1;
				total_size = bank1_size;
			}
		}
	}

	debug("Number of flash banks detected: %d\n", tqm834x_num_flash_banks);

	/* set OR0 and BR0 */
	im->lbus.bank[0].or = CFG_OR_TIMING_FLASH |
		(-(total_size) & OR_GPCM_AM);
	im->lbus.bank[0].br = (CFG_FLASH_BASE & BR_BA) |
		(BR_MS_GPCM | BR_PS_32 | BR_V);

	return (0);
}

/*************************************************************************
 * Detect the size of a ddr bank. Sets CS bounds and CS config accordingly.
 */
static long int get_ddr_bank_size(short cs, volatile long *base)
{
	/* This array lists all valid DDR SDRAM configurations, with
	 * Bank sizes in bytes. (Refer to Table 9-27 in the MPC8349E RM).
	 * The last entry has to to have size equal 0 and is igonred during
	 * autodection. Bank sizes must be in increasing order of size
	 */
	struct {
		long row;
		long col;
		long size;
	} conf[] = {
		{CSCONFIG_ROW_BIT_12,	CSCONFIG_COL_BIT_8,	32 << 20},
		{CSCONFIG_ROW_BIT_12,	CSCONFIG_COL_BIT_9,	64 << 20},
		{CSCONFIG_ROW_BIT_12,	CSCONFIG_COL_BIT_10,	128 << 20},
		{CSCONFIG_ROW_BIT_13,	CSCONFIG_COL_BIT_9,	128 << 20},
		{CSCONFIG_ROW_BIT_13,	CSCONFIG_COL_BIT_10,	256 << 20},
		{CSCONFIG_ROW_BIT_13,	CSCONFIG_COL_BIT_11,	512 << 20},
		{CSCONFIG_ROW_BIT_14,	CSCONFIG_COL_BIT_10,	512 << 20},
		{CSCONFIG_ROW_BIT_14,	CSCONFIG_COL_BIT_11,	1024 << 20},
		{0,			0,			0}
	};

	int i;
	int detected;
	long size;

	detected = -1;
	for(i = 0; conf[i].size != 0; ++i) {

		/* set sdram bank configuration */
		set_cs_config(cs, CSCONFIG_EN | conf[i].col | conf[i].row);

		debug("Getting RAM size...\n");
		size = get_ram_size(base, DDR_MAX_SIZE_PER_CS);

		if((size == conf[i].size) && (i == detected + 1))
			detected = i;

		debug("Trying %ld x %ld (%ld MiB) at addr %p, detected: %ld MiB\n",
			conf[i].row,
			conf[i].col,
			conf[i].size >> 20,
			base,
			size >> 20);
	}

	if(detected == -1){
		/* disable empty cs */
		debug("\nNo valid configurations for CS%d, disabling...\n", cs);
		set_cs_config(cs, 0);
		return 0;
	}
		
	debug("\nDetected configuration %ld x %ld (%ld MiB) at addr %p\n",
			conf[detected].row, conf[detected].col, conf[detected].size >> 20, base);
	
	/* configure cs ro detected params */
	set_cs_config(cs, CSCONFIG_EN | conf[detected].row |
			conf[detected].col);

	set_cs_bounds(cs, (long)base, conf[detected].size);

	return(conf[detected].size);
}

/**************************************************************************
 * Sets DDR bank CS bounds.
 */
static void set_cs_bounds(short cs, long base, long size)
{
	debug("Setting bounds %08x, %08x for cs %d\n", base, size, cs);
	if(size == 0){
		im->ddr.csbnds[cs].csbnds = 0x00000000;
	} else {
		im->ddr.csbnds[cs].csbnds =
			((base >> CSBNDS_SA_SHIFT) & CSBNDS_SA) |
			(((base + size - 1) >> CSBNDS_EA_SHIFT) &
				CSBNDS_EA);
	}
	SYNC;
}

/**************************************************************************
 * Sets DDR banks CS configuration.
 * config == 0x00000000 disables the CS.
 */
static void set_cs_config(short cs, long config)
{
	debug("Setting config %08x for cs %d\n", config, cs);
	im->ddr.cs_config[cs] = config;
	SYNC;
}

/**************************************************************************
 * Sets DDR clocks, timings and configuration.
 */
static void set_ddr_config(void) {
	/* clock control */
	im->ddr.sdram_clk_cntl = DDR_SDRAM_CLK_CNTL_SS_EN |
		DDR_SDRAM_CLK_CNTL_CLK_ADJUST_05;
	SYNC;
	
	/* timing configuration */
	im->ddr.timing_cfg_1 =
		(4 << TIMING_CFG1_PRETOACT_SHIFT) |
		(7 << TIMING_CFG1_ACTTOPRE_SHIFT) |
		(4 << TIMING_CFG1_ACTTORW_SHIFT)  |
		(5 << TIMING_CFG1_REFREC_SHIFT)   |
		(3 << TIMING_CFG1_WRREC_SHIFT)    |
		(3 << TIMING_CFG1_ACTTOACT_SHIFT) |
		(1 << TIMING_CFG1_WRTORD_SHIFT)   |
		(TIMING_CFG1_CASLAT & TIMING_CASLAT);

	im->ddr.timing_cfg_2 =
		TIMING_CFG2_CPO_DEF |
		(2 << TIMING_CFG2_WR_DATA_DELAY_SHIFT);
	SYNC;

	/* don't enable DDR controller yet */
	im->ddr.sdram_cfg =
		SDRAM_CFG_SREN |
		SDRAM_CFG_SDRAM_TYPE_DDR;
	SYNC;
	
	/* Set SDRAM mode */
	im->ddr.sdram_mode =
		((DDR_MODE_EXT_MODEREG | DDR_MODE_WEAK) <<
			SDRAM_MODE_ESD_SHIFT) |
		((DDR_MODE_MODEREG | DDR_MODE_BLEN_4) <<
			SDRAM_MODE_SD_SHIFT) |
		((DDR_MODE_CASLAT << SDRAM_MODE_SD_SHIFT) &
			MODE_CASLAT);
	SYNC;

	/* Set fast SDRAM refresh rate */
	im->ddr.sdram_interval =
		(DDR_REFINT_166MHZ_7US << SDRAM_INTERVAL_REFINT_SHIFT) |
		(DDR_BSTOPRE << SDRAM_INTERVAL_BSTOPRE_SHIFT);
	SYNC;
}
