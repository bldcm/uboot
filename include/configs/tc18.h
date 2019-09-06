/*
 * (C) Copyright 2005
 * Stefan Roese, DENX Software Engineering, sr@denx.de.
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

/************************************************************************
 * tc18.h - configuration for TC18 board
 ***********************************************************************/
#ifndef __CONFIG_H
#define __CONFIG_H

/*
* Boot Flags
 */
#define BOOT_EEPROM			/* Normal Power-On: Boot from EEPROM	*/
#undef 	BOOT_FLASH			/* Normal Power-On: Boot from FLASH	*/

#define VERSION "3.02.02"
#define CONFIG_IDENT_STRING " TCBios "VERSION 

#define DEBUG

/*-----------------------------------------------------------------------
 * High Level Configuration Options
 *----------------------------------------------------------------------*/
#define CONFIG_TCLS1		1	/* Board is Topcall Board       */
#define CONFIG_TC18		1	/* Board is TC18         */
#define CONFIG_440GR		1	/* Specific PPC440GR support    */
#define CONFIG_4xx		1	/* ... PPC4xx family	        */
#define CONFIG_SYS_CLK_FREQ	33333333 //66666666    /* external freq to pll	*/

#define CONFIG_BOARD_EARLY_INIT_F 1     /* Call board_early_init_f	*/
//#define CONFIG_MISC_INIT_R	1	/* call misc_init_r()		*/

/*-----------------------------------------------------------------------
 * Base addresses -- Note these are effective addresses where the
 * actual resources get mapped (not physical addresses)
 *----------------------------------------------------------------------*/
#define CFG_MONITOR_LEN		(512 * 1024)	/* Reserve 512 kB for Monitor	*/
#define CFG_MALLOC_LEN		(256 * 1024)	/* Reserve 256 kB for malloc()	*/
#ifdef CFG_BOOT_RAM_TEST
	#define CFG_MONITOR_BASE	1000000
#else
	#define CFG_MONITOR_BASE	(-CFG_MONITOR_LEN)
#endif
#define CFG_SDRAM_BASE	        0x00000000	    /* _must_ be 0	*/
#define CFG_PCI_MEMBASE	        0xa0000000	    /* mapped pci memory*/
#define CFG_PCI_MEMBASE1        CFG_PCI_MEMBASE  + 0x10000000
#define CFG_PCI_MEMBASE2        CFG_PCI_MEMBASE1 + 0x10000000
#define CFG_PCI_MEMBASE3        CFG_PCI_MEMBASE2 + 0x10000000

/*Don't change either of these*/
#define CFG_PERIPHERAL_BASE     0xef600000	    /* internal peripherals*/
#define CFG_PCI_BASE	        0xe0000000	    /* internal PCI regs*/
/*Don't change either of these*/

#define CFG_USB_DEVICE          0x50000000
#define CFG_NVRAM_BASE_ADDR     0x80000000
#define CFG_BCSR_BASE	        (CFG_NVRAM_BASE_ADDR | 0x2000)
#define CFG_BOOT_BASE_ADDR      0xf0000000

/*-----------------------------------------------------------------------
 * Initial RAM & stack pointer (placed in SDRAM)
 *----------------------------------------------------------------------*/
#define CFG_INIT_RAM_ADDR	0x70000000		/* DCache       */
#define CFG_INIT_RAM_END	(8 << 10)
#define CFG_GBL_DATA_SIZE	256			/* num bytes initial data*/
#define CFG_GBL_DATA_OFFSET	(CFG_INIT_RAM_END - CFG_GBL_DATA_SIZE)
#define CFG_INIT_SP_OFFSET	CFG_GBL_DATA_OFFSET

/*-----------------------------------------------------------------------
 * Serial Port
 *----------------------------------------------------------------------*/
//#undef  CFG_EXT_SERIAL_CLOCK	/* not use external */
#ifndef CONFIG_BAUDRATE
#define CONFIG_BAUDRATE		115200  //9600
#endif
#define CONFIG_SERIAL_MULTI     1
/*define this if you want console on UART1*/
#undef  CONFIG_UART1_CONSOLE

#define CFG_BAUDRATE_TABLE  \
    {300, 600, 1200, 2400, 4800, 9600, 19200, 38400, 57600, 115200}

/*-----------------------------------------------------------------------
 * Environment
 *----------------------------------------------------------------------*/
/*
 * Define here the location of the environment variables (FLASH or EEPROM).
 * Note: DENX encourages to use redundant environment in FLASH.
 */
//#if 1
#define CFG_ENV_IS_IN_FLASH     1	/* use FLASH for environment vars	*/
#undef  CFG_NO_FLASH	
//#endif

//#if 0
//#define CFG_ENV_IS_IN_EEPROM	1	/* use EEPROM for environment vars	*/
//#endif

//#if 0
//  #define CFG_NO_FLASH		1	/* Flash is not usable now */
//  #define CFG_ENV_IS_NOWHERE	1	/* Store ENV in memory only */
//  #define CFG_ENV_ADDR		(CFG_MONITOR_BASE - 0x1000)
//  #define CFG_ENV_SIZE		0x2000
//#endif



/* Define to allow the user to overwrite serial and ethaddr */
#define CONFIG_ENV_OVERWRITE

/*-----------------------------------------------------------------------
 * FLASH related
 *----------------------------------------------------------------------*/
#ifndef CFG_NO_FLASH

#define CFG_FLASH_BASE		0x90000000
#define CFG_FLASH_INCREMENT	0x01000000

#define CFG_FLASH_CFI         1       /* Flash is CFI conformant */
#define CFG_FLASH_CFI_DRIVER  1       /* Use the common driver */
#define CFG_FLASH_CFI_AMD_RESET 1     /* AMD RESET for STM 29W320DB!	*/
//#define CFG_FLASH_PROTECTION  1       /* don't use hardware protection        */
//#define CFG_FLASH_USE_BUFFER_WRITE 1  /* use buffered writes (20x faster)     */
#define CFG_MAX_FLASH_BANKS   2       /* max num of flash banks */
#define CFG_FLASH_BANKS_LIST { CFG_FLASH_BASE, CFG_FLASH_BASE + CFG_FLASH_INCREMENT }
#define CFG_MAX_FLASH_SECT    512     /* max num of sects on one chip */


#define CFG_FLASH_ERASE_TOUT	120000	/* Timeout for Flash Erase (in ms)	*/
#define CFG_FLASH_WRITE_TOUT	500	/* Timeout for Flash Write (in ms)	*/

#define CFG_FLASH_EMPTY_INFO		/* print 'E' for empty sector on flinfo */

#ifdef CFG_ENV_IS_IN_FLASH
#define CFG_ENV_SECT_SIZE	0x20000 	/* size of one complete sector	*/
#define	CFG_ENV_SIZE		0x2000		/* Total Size of Environment Sector	*/
#define	CFG_ENV_NUMBER		4		/* max. number of environment variant	*/
#define CFG_ENV_OFFSET          0x0 /* Offset of Environment Sector  */
/* Address and size of Redundant Environment Sector	*/
//#define CFG_ENV_ADDR_REDUND	(CFG_ENV_ADDR-CFG_ENV_SECT_SIZE)
//#define CFG_ENV_SIZE_REDUND	(CFG_ENV_SIZE)
#endif /* CFG_ENV_IS_IN_FLASH */

#endif
/*-----------------------------------------------------------------------
 * DDR SDRAM
 *----------------------------------------------------------------------*/
#undef CONFIG_SPD_EEPROM	       /* Don't use SPD EEPROM for setup    */
#define CFG_KBYTES_SDRAM        (1 * 128 * 1024)    /* 128MB		    */
#define CFG_SDRAM_BANKS	        (1)

#define  CFG_DRAM_TEST

/*-----------------------------------------------------------------------
 * I2C
 *----------------------------------------------------------------------*/
#define CONFIG_HARD_I2C		1	    /* I2C with hardware support	*/
#undef	CONFIG_SOFT_I2C			    /* I2C bit-banged		*/
#define CFG_I2C_SPEED		400000	/* I2C speed and slave address	*/
#define CFG_I2C_SLAVE		0x7F

//#define CFG_I2C_MULTI_EEPROMS
//#define CFG_I2C_EEPROM_ADDR	(0xa8>>1)
//#define CFG_I2C_EEPROM_ADDR_LEN 1
#define CFG_EEPROM_PAGE_WRITE_ENABLE
#define CFG_EEPROM_PAGE_WRITE_BITS 3
#define CFG_EEPROM_PAGE_WRITE_DELAY_MS 10

#ifdef CFG_ENV_IS_IN_EEPROM
#define CFG_ENV_SIZE		0x1000	    /* Size of Environment vars */
#define CFG_ENV_OFFSET		0x0
#endif /* CFG_ENV_IS_IN_EEPROM */

#undef	CONFIG_BOOTARGS

#if 0
#define CONFIG_BOOTDELAY	-1	/* autoboot disabled		*/
#else
#define CONFIG_BOOTDELAY	5	/* autoboot after 5 seconds	*/
#endif

#undef CONFIG_LOADS_ECHO		/* echo on for serial download	*/
#define CFG_LOADS_BAUD_CHANGE	1	/* allow baudrate change	*/

#define CONFIG_MII		1	/* MII PHY management		*/
#define CONFIG_NET_MULTI        1	/* required for netconsole      */

#define CONFIG_HAS_ETH1		1	/* add support for "eth1addr"	*/
#define CONFIG_PHY_ADDR		1	/* PHY address, See schematics	*/
#define CONFIG_PHY1_ADDR        2 

//#define CONFIG_IPADDR	192.168.0.10
//#define CONFIG_SERVERIP 192.168.0.1
//#define CONFIG_NETMASK  255.255.255.0
//#define CONFIG_HOSTNAME	TC18
//#define CONFIG_ETHADDR		00:01:ec:00:89:6b
//#define CONFIG_ETH1ADDR		00:01:ec:80:89:6b

#define CFG_RX_ETH_BUFFER	32	  /* Number of ethernet rx buffers & descriptors */

/* Partitions */
#define CONFIG_MAC_PARTITION
#define CONFIG_DOS_PARTITION
#define CONFIG_ISO_PARTITION

#ifdef CONFIG_440EP
/* USB */
#define CONFIG_USB_OHCI
#define CONFIG_USB_STORAGE

/*Comment this out to enable USB 1.1 device*/
#define USB_2_0_DEVICE
#endif /*CONFIG_440EP*/

#ifdef DEBUG
#define CONFIG_PANIC_HANG
#else
#define CONFIG_HW_WATCHDOG			/* watchdog */
#endif

#ifdef CFG_NO_FLASH

#define REMOVE_COMMANDS	       (CFG_CMD_FLASH | CFG_CMD_IMLS )

#define CONFIG_COMMANDS	       ((CONFIG_CMD_DFL	| \
				CFG_CMD_ASKENV	| \
				CFG_CMD_DHCP	| \
				CFG_CMD_DIAG	| \
				CFG_CMD_ELF	| \
				CFG_CMD_I2C	| \
				CFG_CMD_IRQ	| \
				CFG_CMD_MII	| \
				CFG_CMD_NET	| \
				CFG_CMD_NFS	| \
				CFG_CMD_PCI	| \
				CFG_CMD_PING	| \
				CFG_CMD_BSP	| \
				CFG_CMD_REGINFO	| \
				CFG_CMD_SDRAM) & ~REMOVE_COMMANDS)
#else

#define CONFIG_COMMANDS	       (CONFIG_CMD_DFL	| \
				CFG_CMD_ASKENV	| \
				CFG_CMD_DHCP	| \
				CFG_CMD_DIAG	| \
				CFG_CMD_ELF	| \
				CFG_CMD_I2C	| \
				CFG_CMD_IRQ	| \
				CFG_CMD_MII	| \
				CFG_CMD_NET	| \
				CFG_CMD_NFS	| \
				CFG_CMD_PCI	| \
				CFG_CMD_PING	| \
				CFG_CMD_FLASH 	| \
				CFG_CMD_BSP	| \
				CFG_CMD_REGINFO	| \
				CFG_CMD_SDRAM)

#endif

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

/*
 * Miscellaneous configurable options
 */
#define CFG_LONGHELP			/* undef to save memory		*/
#define CFG_PROMPT	        "=> "	/* Monitor Command Prompt	*/
#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CFG_CBSIZE	        1024	/* Console I/O Buffer Size	*/
#else
#define CFG_CBSIZE	        1024	/* Console I/O Buffer Size	*/
#endif
#define CFG_PBSIZE              (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define CFG_MAXARGS	        16	/* max number of command args	*/
#define CFG_BARGSIZE	        CFG_CBSIZE /* Boot Argument Buffer Size	*/

#define CFG_MEMTEST_START	0x0400000 /* memtest works on	        */
#define CFG_MEMTEST_END		0x0C00000 /* 4 ... 12 MB in DRAM	*/

#define CFG_LOAD_ADDR		0x100000	/* default load address */
#define CFG_EXTBDINFO		1	/* To use extended board_into (bd_t) */
#define CONFIG_LYNXKDI          1       /* support kdi files            */

#define CFG_HZ		        1000	/* decrementer freq: 1 ms ticks */

/*-----------------------------------------------------------------------
 * PCI stuff
 *-----------------------------------------------------------------------
 */
/* General PCI */
#define CONFIG_PCI			/* include pci support	        */
#define CONFIG_PCI_PNP			/* do  pci plug-and-play   */
#define CONFIG_PCI_SCAN_SHOW            /* show pci devices on startup  */
#define CFG_PCI_TARGBASE        0xa0000000 /* PCIaddr mapped to CFG_PCI_MEMBASE*/

/* Board-specific PCI */
#define CFG_PCI_PRE_INIT                /* enable board pci_pre_init()  */
#define CFG_PCI_TARGET_INIT
#define CFG_PCI_MASTER_INIT

#define CFG_PCI_SUBSYS_VENDORID 0x10e8	/* AMCC */
#define CFG_PCI_SUBSYS_ID       0xcafe	/* Whatever */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CFG_BOOTMAPSZ		(8 << 20)	/* Initial Memory map for Linux */

/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CFG_DCACHE_SIZE		(32<<10) /* For AMCC 440 CPUs			*/
#define CFG_CACHELINE_SIZE	32	/* ...			*/
#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CFG_CACHELINE_SHIFT	5	/* log base 2 of the above value	*/
#endif

/*
 * Internal Definitions
 *
 * Boot Flags
 */

#define BOOTFLAG_COLD	0x01		/* Normal Power-On: Boot from FLASH/EEPROM	*/
#define BOOTFLAG_WARM	0x02		/* Software reboot			*/

#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE	230400	/* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX	2	/* which serial port to use */
#endif

#endif	/* __CONFIG_H */
