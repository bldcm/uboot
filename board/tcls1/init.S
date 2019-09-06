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

//-----------------------------------------------------------------------------
// Function:     ext_bus_cntlr_init
// Description:  Initializes the External Bus Controller for the external
//              peripherals. IMPORTANT: For pass1 this code must run from
//              cache since you can not reliably change a peripheral banks
//              timing register (pbxap) while running code from that bank.
//              For ex., since we are running from ROM on bank 0, we can NOT
//              execute the code that modifies bank 0 timings from ROM, so
//              we run it from cache.
//      Bank 0 - Flash and SRAM
//      Bank 1 - NVRAM/RTC
//      Bank 2 - Keyboard/Mouse controller
//      Bank 3 - IR controller
//      Bank 4 - not used
//      Bank 5 - not used
//      Bank 6 - not used
//      Bank 7 - FPGA registers
//-----------------------------------------------------------------------------#include <config.h>
#include <ppc4xx.h>

#define _LINUX_CONFIG_H 1       /* avoid reading Linux autoconf.h file  */
#define FPGA_BRDC       0xF0300004

#include <ppc_asm.tmpl>
#include <ppc_defs.h>

#include <asm/cache.h>
#include <asm/mmu.h>


        .globl  ext_bus_cntlr_init
ext_bus_cntlr_init:

#if 0
        mflr    r4                      // save link register
        bl      ..getAddr
..getAddr:
        mflr    r3                      // get address of ..getAddr
        mtlr    r4                      // restore link register
        addi    r4,0,14                 // set ctr to 10; used to prefetch
        mtctr   r4                      // 10 cache lines to fit this function
                                        // in cache (gives us 8x10=80 instrctns)
..ebcloop:
        icbt    r0,r3                   // prefetch cache line for addr in r3
        addi    r3,r3,32                // move to next cache line
        bdnz    ..ebcloop               // continue for 10 cache lines

        //-------------------------------------------------------------------
        // Delay to ensure all accesses to ROM are complete before changing
        // bank 0 timings. 200usec should be enough.
        //   200,000,000 (cycles/sec) X .000200 (sec) = 0x9C40 cycles
        //-------------------------------------------------------------------
        addis   r3,0,0x0
        ori     r3,r3,0xA000          // ensure 200usec have passed since reset
        mtctr   r3
..spinlp:
        bdnz    ..spinlp                // spin loop

        //-----------------------------------------------------------------------
        // Memory Bank 0 (Flash and SRAM) initialization
        //-----------------------------------------------------------------------
        addi    r4,0,pb0ap
        mtdcr   ebccfga,r4
        addis   r4,0,0x9B01
        ori     r4,r4,0x5480
        mtdcr   ebccfgd,r4

        addi    r4,0,pb0cr
        mtdcr   ebccfga,r4
        addis   r4,0,0xFFF1           // BAS=0xFFF,BS=0x0(1MB),BU=0x3(R/W),
        ori     r4,r4,0x8000          // BW=0x0( 8 bits)
        mtdcr   ebccfgd,r4

#endif  /* #if 0 */

        //----------------------------------------------------------------------
        // set general EBC configuration (EBC0_CFG register)
        //----------------------------------------------------------------------
        // See NPe405L User's manual, page 14-17
        addi    r4,0,epcr
        mtdcr   ebccfga,r4
        addis   r4,0,0xc040           // EBCT=1,PTD=1,RTC=000,res=0000,
                                      // CSTC=1,BPF=00,res=00,PME=0,PMT=0
#if 0
later , we actually *want* timeouts on EB, so uncomment this:
        addis   r4,0,0xb840           // EBCT=1,PTD=0,RTC=111,res=0000,
                                      // CSTC=1,BPF=00,res=00,PME=0,PMT=0
        ori     r4,r4,0x0000          // PMT=000,res=0x000
(2048 PerClk cycles should be 25MHz, that makes it 81,92 usec)
#endif
        mtdcr   ebccfgd,r4


        //---------------------------------------------------------------------
        // Memory Bank 1 (Flash at 40000000) 
        //---------------------------------------------------------------------

/* See diagrams in NPe405L User's manual, page 14-19

   BME=0 (burst mode disabled)
   TWT=4 (total TWT+1=5 cycles)
   CSN=1 (#of cycles between address driven and chip select low, could
          be 0 as well).
   OEN=1 (#of cycles between chip select and output enable, for reading).
   WBN=1 (#of cycles between chip select and write enable, for writing).
   WBF=1 (#of cycles between write enable off and chip select off , for writing).
   RE =0 (PerReady is disabled. )
   SOR=0 (doesn't apply).
   BEM=0 (byte enable, this controls behaviour of PerWBE).
   PEN=0 (no parity checking). */

        addi    r4,0,pb1ap
        mtdcr   ebccfga,r4
        addis   r4,0,0x0205        // BME=0,TWT=4,CSN=1,OEN=1
        ori     r4,r4,0x5200       // WBN=1,WBF=1,TH=1,RE=0,SOR=0,BEM=0,PEN=0
        mtdcr   ebccfgd,r4

        addi    r4,0,pb1cr
        mtdcr   ebccfga,r4
        addis   r4,0,0x4009        // BAS=0xF00,BS=100b (16MB),BU=0x3(R/W),
        ori     r4,r4,0xA000       // BW=01b (16 bits)
/* Uncomment this to configure flash as 8-bit device for flash A10 
   wordaround. */
        // ori     r4,r4,0x8000       // BW=00b (16 bits)
        mtdcr   ebccfgd,r4

        //-----------------------------------------------------------------------
        // Memory Bank 2 (4x DSP at f0000000)
        //-----------------------------------------------------------------------

/*
 * The DSP's are pretty fast, however let's get started with some
 * easy timing .. I took the ones from above for now ..
 */
        addi    r4,0,pb2ap
        mtdcr   ebccfga,r4
        addis   r4,0,0x0205        // BME=0,TWT=4,CSN=1,OEN=1
        ori     r4,r4,0x1300       // WBN=0,WBF=1,TH=1,RE=1,SOR=0,BEM=0,PEN=0
        mtdcr   ebccfgd,r4

        addi    r4,0,pb2cr
        mtdcr   ebccfga,r4
        addis   r4,0,0xF001        // BAS=0xF00,BS=0x0(1MB),BU=0x3(R/W),
        ori     r4,r4,0xA000       // BW=01b (16 bits)
        mtdcr   ebccfgd,r4

        //----------------------------------------------------------------------
        // Memory Bank 3 (EPLD at f0100000)
        //----------------------------------------------------------------------

        addi    r4,0,pb3ap
        mtdcr   ebccfga,r4
        addis   r4,0,0x0205        // BME=0,TWT=4,CSN=1,OEN=1
        ori     r4,r4,0x5200       // WBN=1,WBF=1,TH=1,RE=0,SOR=0,BEM=0,PEN=0
        mtdcr   ebccfgd,r4

        addi    r4,0,pb3cr
        mtdcr   ebccfga,r4
        addis   r4,0,0xF011             // BAS=0xF01,BS=0x0(1MB),BU=0x3(R/W),
        ori     r4,r4,0xA000            // BW=01b(16 bit device)
        mtdcr   ebccfgd,r4

        blr

//-----------------------------------------------------------------------------
// Function:     sdram_init
// Description:  Configures SDRAM memory banks.
//               Auto Memory Configuration option reads the SDRAM EEPROM
//              via the IIC bus and then configures the SDRAM memory
//               banks appropriately. If Auto Memory Configuration is
//              is not used, it is assumed that a 32MB 12x8(2) non-ECC DIMM is
//              plugged, ie. the DIMM that shipped wih the Eval board.
//-----------------------------------------------------------------------------
        .globl  sdram_init

sdram_init:

        mflr    r31

        //-------------------------------------------------------------------
        // Set MB0CF for bank 0. (0-32MB) Address Mode 2 since 12x9(4)
        //-------------------------------------------------------------------
        addi    r4,0,mem_mb0cf
        mtdcr   memcfga,r4
        addis   r4,0,0x0006
        ori     r4,r4,0x2001
        mtdcr   memcfgd,r4


/* This setting assumes MemClk == 100MHz */

        addis   r6,0,0x0086            // SDTR1 value for 100Mhz
        ori     r6,r6,0x400D
        addis   r7,0,0x05F0            // RTR value for 100Mhz

        //-------------------------------------------------------------------
        // Set SDTR1
        //-------------------------------------------------------------------
        addi    r4,0,mem_sdtr1
        mtdcr   memcfga,r4
        mtdcr   memcfgd,r6

        //-------------------------------------------------------------------
        // Set RTR
        //-------------------------------------------------------------------
        addi    r4,0,mem_rtr
        mtdcr   memcfga,r4
        mtdcr   memcfgd,r7

        //-------------------------------------------------------------------
        // Delay to ensure 200usec have elapsed since reset. Assume worst
        // case that the core is running 200Mhz:
        //   200,000,000 (cycles/sec) X .000200 (sec) = 0x9C40 cycles
        //-------------------------------------------------------------------
        addis   r3,0,0x0000
        ori     r3,r3,0xA000          // ensure 200usec have passed since reset
        mtctr   r3
..spinlp2:
        bdnz    ..spinlp2               // spin loop

        //-------------------------------------------------------------------
        // Set memory controller options reg, MCOPT1.
        // Set DC_EN to '1' and BRD_PRF to '01' for 16 byte PLB Burst
        // read/prefetch.
        //-------------------------------------------------------------------
        addi    r4,0,mem_mcopt1
        mtdcr   memcfga,r4
        addis   r4,0,0x8080             // set DC_EN=1
        ori     r4,r4,0x0000
        mtdcr   memcfgd,r4

        //-------------------------------------------------------------------
        // Delay to ensure 10msec have elapsed since reset. This is
        // required for the MPC952 to stabalize. Assume worst
        // case that the core is running 200Mhz:
        //   200,000,000 (cycles/sec) X .010 (sec) = 0x1E8480 cycles
        // This delay should occur before accessing SDRAM.
        //-------------------------------------------------------------------
        addis   r3,0,0x001E
        ori     r3,r3,0x8480          // ensure 10msec have passed since reset
        mtctr   r3
..spinlp3:
        bdnz    ..spinlp3                // spin loop

        mtlr    r31                     // restore lr
        blr
