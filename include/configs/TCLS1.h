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

/*
 * board/config.h - configuration options, board specific
 */

#ifndef __CONFIG_H
#define __CONFIG_H

/*
 * High Level Configuration Options
 * (easy to change)
 */
#define  DEBUG	1			/* debug output code */

#define VERSION "3.00"

#define CONFIG_405L          	1   /* This is a PPC405L network CPU */
#define CONFIG_405      	1	/* This is a PPC405 CPU		*/
#define CONFIG_4xx		1	/* ...member of PPC4xx family   */
#define CONFIG_TCLS1            1       /* ...TOPCALL line server 1 */

//#define CONFIG_LITTLE_ENDIAN    1  /* we're operating in little endian .. */

#define CONFIG_SYS_CLK_FREQ     50000000 /* external frequency to pll   */
        /* 50 MHz */

#define CFG_ENV_IS_IN_FLASH     1       /* use FLASH for environment vars */
//#define CFG_ENV_IS_IN_NVRAM	1	/* use NVRAM for environment vars	*/

#ifdef CFG_ENV_IS_IN_NVRAM
#undef CFG_ENV_IS_IN_FLASH
#else
#ifdef CFG_ENV_IS_IN_FLASH
#undef CFG_ENV_IS_IN_NVRAM
#endif
#endif

#define CONFIG_ENV_OVERWRITE    1  /* Allow overwriting of ethaddr and serial# */

// #define CONFIG_BAUDRATE              19200
 /* Changed in 1.08.00, however I think it is a *bad* idea */
#define CONFIG_BAUDRATE         115200
#define CONFIG_BOOTDELAY        3       /* autoboot after 3 seconds     */

#define CONFIG_BOOTCOMMAND      "lcd ** Not set up **Run diagnostics" /* Copy from Flash to RAM */
#define CONFIG_BOOTCOMMAND_ALT  "bootm 1000000"  /* Boot from RAM */


/* Size (bytes) of interrupt driven serial port buffer.
 * Set to 0 to use polling instead of interrupts.
 * Setting to 0 will also disable RTS/CTS handshaking.
 */
#if 0
#define CONFIG_SERIAL_SOFTWARE_FIFO 4000
#else
#undef	CONFIG_SERIAL_SOFTWARE_FIFO
#endif

#define CONFIG_BOOTARGS         "root=/dev/mem initrd"


#define CONFIG_LOADS_ECHO       1       /* echo on for serial download  */
#define CFG_LOADS_BAUD_CHANGE   1       /* allow baudrate change        */

#define CONFIG_MII		1	/* MII PHY management		*/
#define CONFIG_PHY_ADDR         1       /* PHY address                  */

// ????? #define CONFIG_RTC_DS174x	1	/* use DS1743 RTC in Walnut	*/

#define CONFIG_COMMANDS         \
        ((CONFIG_CMD_DFL | CFG_CMD_IRQ | CFG_CMD_BSP) & ~CFG_CMD_NET & ~CFG_CMD_KGDB)

/* this must be included AFTER the definition of CONFIG_COMMANDS (if any) */
#include <cmd_confdefs.h>

#undef CONFIG_WATCHDOG                  /* watchdog disabled            */

#undef CONFIG_SPD_EEPROM                /* use SPD EEPROM for setup    */

/*
 * Miscellaneous configurable options
 */
#define CFG_LONGHELP                    /* undef to save memory         */
#define CFG_PROMPT      "=> "           /* Monitor Command Prompt       */
#define CFG_CBSIZE      1024            /* Console I/O Buffer Size      */
#define CFG_PBSIZE (CFG_CBSIZE+sizeof(CFG_PROMPT)+16) /* Print Buffer Size */
#define CFG_MAXARGS     16              /* max number of command args   */
#define CFG_BARGSIZE    CFG_CBSIZE      /* Boot Argument Buffer Size    */

#define CFG_MEMTEST_START       0x0400000       /* memtest works on     */
#define CFG_MEMTEST_END         0x0C00000       /* 4 ... 12 MB in DRAM  */

/*
 * If CFG_EXT_SERIAL_CLOCK, then the UART divisor is 1.
 * If CFG_405_UART_ERRATA_59, then UART divisor is 31.
 * Otherwise, UART divisor is determined by CPU Clock and CFG_BASE_BAUD value.
 * The Linux BASE_BAUD define should match this configuration.
 *    baseBaud = cpuClock/(uartDivisor*16)
 * If CFG_405_UART_ERRATA_59 and 200MHz CPU clock,
 * set Linux BASE_BAUD to 403200.
 */
#undef  CFG_EXT_SERIAL_CLOCK           /* no external serial clock used */
#undef  CFG_405_UART_ERRATA_59         /* 405GP/CR Rev. D silicon */
#define CFG_BASE_BAUD       691200

/* The following table includes the supported baudrates */
#define CFG_BAUDRATE_TABLE      \
        { 300, 600, 1200, 2400, 4800, 9600, 19200, 38400,     \
         57600, 115200 }

#define CFG_CLKS_IN_HZ  1               /* everything, incl board info, in Hz */

#define CFG_LOAD_ADDR           0x100000        /* default load address */
#define CFG_EXTBDINFO           1       /* To use extended board_into (bd_t) */

#define CFG_HZ          1000            /* decrementer freq: 1 ms ticks */

#define CONFIG_HARD_I2C		1	/* I2C with hardware support	*/
#undef  CONFIG_SOFT_I2C			/* I2C bit-banged		*/
#define CFG_I2C_SPEED		400000	/* I2C speed and slave address	*/
#define CFG_I2C_SLAVE		0x7F

#define CONFIG_HAS_ETH2		1

/* no PCI */

#undef CONFIG_PCI

/* no IDE as well */

/*-----------------------------------------------------------------------
 * Start addresses for the final memory configuration
 * (Set up by the startup code)
 * Please note that CFG_SDRAM_BASE _must_ start at 0
 */
#define CFG_SDRAM_BASE          0x00000000
#define CFG_SDRAM_SIZE          0x02000000      /* 32 MB */
#define CFG_FLASH_BASE          0x40000000
#if 0
???? make both ???
#undef CFG_FLASH_BASE
#define CFG_FLASH_BASE          0x40300000
#endif
#define CFG_MONITOR_BASE        0xfffe0000
#define CFG_MONITOR_LEN         (192 * 1024)    /* Reserve 192 kB for Monitor    (including .bss section) */
#define CFG_IMAGE_LEN           (128 * 1024)    /* But the image is only 128K */
#define CFG_MALLOC_LEN          (128 * 1024)    /* Reserve 128 kB for malloc()  */

/*
 * For booting Linux, the board info and command line data
 * have to be in the first 8 MB of memory, since this is
 * the maximum mapped by the Linux kernel during initialization.
 */
#define CFG_BOOTMAPSZ           (8 << 20)       /* Initial Memory map for Linux */
/*-----------------------------------------------------------------------
 * FLASH organization
 */
#define CFG_MAX_FLASH_BANKS     2       /* max number of memory banks           */
#define CFG_MAX_FLASH_SECT      256     /* max number of sectors on one chip    */

#define CFG_FLASH_ERASE_TOUT    120000  /* Timeout for Flash Erase (in ms)      */
#define CFG_FLASH_WRITE_TOUT    500     /* Timeout for Flash Write (in ms)      */

/* BEG ENVIRONNEMENT FLASH */
#ifdef CFG_ENV_IS_IN_FLASH
 /* Note: with Intel flashes (which are top boot sect, offset changes
    see badly crafted code in common/cmd_nvedit.c .. */

#define CFG_ENV_OFFSET          0x0 /* Offset of Environment Sector  */
/* ?? have multiple sectors for env */
#define CFG_ENV_SIZE            0x1000  /* Total Size of Environment Sector     */
#define CFG_ENV_SECT_SIZE       0x4000  /* see README - env sector total size   */
#endif
/* END ENVIRONNEMENT FLASH */
/*-----------------------------------------------------------------------
 * Cache Configuration
 */
#define CFG_DCACHE_SIZE		8192	/* For IBM 405 CPUs			*/
#define CFG_CACHELINE_SIZE	16	/* ...			*/
#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CFG_CACHELINE_SHIFT     4       /* log base 2 of the above value        */
#endif

/*
 * Init Memory Controller:
 *
 * BR0/1 and OR0/1 (FLASH)
 */

#define FLASH_BASE0_PRELIM      0x40000000      /* FLASH bank #0        */

#if 0
/* We'll fix it .. back to autodetection once flash address lines are
   right .. ?? */
#define FLASH_BASE0_PRELIM      0x40300000      /* FLASH bank #0        */
#endif

// #define FLASH_BASE1_PRELIM   0x40400000      /* FLASH bank #1        */
/* CHANGED to 40800000 in redesigned board ... */
#define FLASH_BASE1_PRELIM      0x40800000      /* FLASH bank #1        */

/* This is the fixed address, the address of bank 0 will be relocated such
   that there is no gap between bank 0 and bank 1. If we have 2MB flash
   chips (16MBit), bank 0 would be at address 40600000, for example.
   A total of 16MByte (2x 8MByte, or 64MBit chips) is supported by TC16 ..
 */

/*-----------------------------------------------------------------------
 * Definitions for initial stack pointer and data area (in DPRAM)
 */
#define CFG_INIT_DCACHE_CS      4       /* use cs # 4 for data cache memory    */

#define CFG_INIT_RAM_ADDR       0x00100000  /* inside of SDRAM                   @1MB out of the way of exception vectors.  */
#define CFG_INIT_RAM_END        0x0f00  /* End of used area in RAM             */
#define CFG_INIT_DATA_SIZE      64  /* size in bytes reserved for initial data */
#define CFG_INIT_DATA_OFFSET    (CFG_INIT_RAM_END - CFG_INIT_DATA_SIZE)
#define CFG_GBL_DATA_SIZE      64  /* size in bytes reserved for initial data */
#define CFG_GBL_DATA_OFFSET    (CFG_INIT_RAM_END - CFG_GBL_DATA_SIZE)
#define CFG_INIT_SP_OFFSET      CFG_INIT_DATA_OFFSET

/*-----------------------------------------------------------------------
 * Definitions for Serial Presence Detect EEPROM address
 * (to get SDRAM settings)
 */

#define SPD_EEPROM_ADDRESS      0x50

#define EEPROM_WRITE_ADDRESS 0xA0
#define EEPROM_READ_ADDRESS  0xA1

/*
 * Internal Definitions
 *
 * Boot Flags
 */
#define BOOTFLAG_COLD   0x01            /* Normal Power-On: Boot from FLASH     */
#define BOOTFLAG_WARM   0x02            /* Software reboot                      */

#if (CONFIG_COMMANDS & CFG_CMD_KGDB)
#define CONFIG_KGDB_BAUDRATE    230400  /* speed to run kgdb serial port */
#define CONFIG_KGDB_SER_INDEX   2       /* which serial port to use */
#endif
#endif  /* __CONFIG_H */
