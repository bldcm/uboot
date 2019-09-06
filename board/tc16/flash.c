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
 * Modified 4/5/2001
 * Wait for completion of each sector erase command issued
 * 4/5/2001
 * Chris Hallinan - DS4.COM, Inc. - clh@net1plus.com
 */

#include <common.h>
#include <ppc4xx.h>
#include <asm/processor.h>
#include <environment.h>

flash_info_t	flash_info[CFG_MAX_FLASH_BANKS]; /* info for FLASH chips	*/
/*-----------------------------------------------------------------------
 * Functions
 */
static int write_word (flash_info_t *info, ulong dest, ulong data);
static ulong flash_get_size (unsigned long base, flash_info_t *info);

#define ADDR0           0x0aaa
#define ADDR1           0x0555
#define FLASH_WORD_SIZE unsigned short
#define FLASH_CTRL_WORD_SIZE unsigned char

/*-----------------------------------------------------------------------
 */


unsigned long flash_init (void)
{
	unsigned long size_b0=0, size_b1=0, base_b0;
	int i;


/* Right now, we have to un-reset flash by software .. */
	volatile long *gpio_or = (volatile long *)0xef600700;


	(*gpio_or) |= 0x200;
	for (i=0;i<1000000;i++) ;


	for (i=0; i<CFG_MAX_FLASH_BANKS; ++i) {
		flash_info[i].flash_id = FLASH_UNKNOWN;
	}

	size_b0 = flash_get_size(FLASH_BASE0_PRELIM, &flash_info[0]);

	if (flash_info[0].flash_id == FLASH_UNKNOWN) {
		printf ("## Unknown FLASH on Bank 0 - Size = 0x%08lx = %ld MB\n",
			size_b0, size_b0<<20);
	}


//	flash_print_info (&flash_info[0]);      /* Print recognized flash type and sector addresses */

	if (CFG_MAX_FLASH_BANKS == 2)
	{
		size_b1 = flash_get_size(FLASH_BASE1_PRELIM, &flash_info[1]);
/* .. do again for bank 0 so that there is no gap .. */
		if (flash_info[1].flash_id != FLASH_UNKNOWN) {
			base_b0 = FLASH_BASE1_PRELIM - size_b0;
			if (base_b0 != FLASH_BASE0_PRELIM) {
		        	size_b0 = flash_get_size(base_b0, &flash_info[0]);
				printf ("Relocated Bank 0 to %08lx, so that there is no gap inbetween ...\n", base_b0);
			}
		}
	}

//	flash_print_info (&flash_info[1]);     /* Print recognized flash type and sector addresses */

	return (size_b0 + size_b1);
}



/*-----------------------------------------------------------------------
 */
void flash_print_info  (flash_info_t *info)
{
	int i;
        int k;
        int size;
        int erased;
        volatile unsigned long *flash;

	if (info->flash_id == FLASH_UNKNOWN) {
		printf ("missing or unknown FLASH type\n");
		return;
	}

	switch (info->flash_id & FLASH_VENDMASK) {
	case FLASH_MAN_AMD:	printf ("AMD ");		break;
	case FLASH_MAN_FUJ:	printf ("FUJITSU ");		break;
	case FLASH_MAN_SST:	printf ("SST ");		break;
	case FLASH_MAN_STM:	printf ("STM ");		break;
	case FLASH_MAN_INTEL:	printf ("Intel ");		break;
	default:		printf ("Unknown Vendor ");	break;
	}

	switch (info->flash_id & FLASH_TYPEMASK) {
	case FLASH_AM040:	printf ("AM29F040 (512 Kbit, uniform sector size)\n");
				break;
	case FLASH_AM400B:	printf ("AM29LV400B (4 Mbit, bottom boot sect)\n");
				break;
	case FLASH_AM400T:	printf ("AM29LV400T (4 Mbit, top boot sector)\n");
				break;
	case FLASH_AM800B:	printf ("AM29LV800B (8 Mbit, bottom boot sect)\n");
				break;
	case FLASH_AM800T:	printf ("AM29LV800T (8 Mbit, top boot sector)\n");
				break;
	case FLASH_AM160B:	printf ("AM29LV160B (16 Mbit, bottom boot sect) or compatible.\n");
				break;
	case FLASH_AM160T:	printf ("AM29LV160T (16 Mbit, top boot sector)  or compatible.\n");
				break;
	case FLASH_AM320B:	printf ("AM29LV320B (32 Mbit, bottom boot sect) or compatible.\n");
				break;
	case FLASH_AM320T:	printf ("AM29LV320T (32 Mbit, top boot sector) or compatible.\n");
				break;
	case FLASH_SST800A:	printf ("SST39LF/VF800 (8 Mbit, uniform sector size)\n");
				break;
	case FLASH_SST160A:	printf ("SST39LF/VF160 (16 Mbit, uniform sector size)\n");
				break;
	case FLASH_28F320J3A:   printf ("INTEL 28F320J3A (32MBit, 128KB sectors)");
			        break;
	case FLASH_28F640J3A:   printf ("INTEL 28F640J3A (64MBit, 128KB sectors)");
			        break;
	case FLASH_28F128J3A:   printf ("INTEL 28F128J3A (128MBit, 128KB sectors)");
			        break;
	case FLASH_28F320C3T:   printf ("INTEL 28F320C3T (32MBit, 63x64KB sectors, 8x8KB top boot sectors)");
			        break;
	case FLASH_ST640B:	printf ("M29W640DB (64 Mbit, bottom boot sector)\n");
				break;
	case FLASH_ST640T:	printf ("M29W640DT (64 Mbit, top boot sector)\n");
				break;
	default:		printf ("Unknown Chip Type\n");
				break;
	}

	printf ("  Size: %ld KB in %d Sectors\n",
		info->size >> 10, info->sector_count);

	printf ("  Sector Start Addresses:");
	for (i=0; i<info->sector_count; ++i) {
                /*
                 * Check if whole sector is erased
                 */
                if (i != (info->sector_count-1))
                  size = info->start[i+1] - info->start[i];
                else
                  size = info->start[0] + info->size - info->start[i];
                erased = 1;
                flash = (volatile unsigned long *)info->start[i];
                size = size >> 2;        /* divide by 4 for longword access */
                for (k=0; k<size; k++)
                  {
                    if (*flash++ != 0xffffffff)
                      {
                        erased = 0;
                        break;
                      }
                  }

		if ((i % 5) == 0)
			printf ("\n   ");
#if 0 /* test-only */
		printf (" %08lX%s",
			info->start[i],
			info->protect[i] ? " (RO)" : "     "
#else
		printf (" %08lX%s%s",
			info->start[i],
			erased ? " E" : "  ",
			info->protect[i] ? "RO " : "   "
#endif
		);
	}
	printf ("\n");
}

/*-----------------------------------------------------------------------
 */

#define SWAP16(x) ((((x) & 0x00ff) << 8) | (((x) & 0xff00) >> 8))

/*-----------------------------------------------------------------------
 */

/*
 * The following code cannot be run from FLASH!
 */
static ulong flash_get_size (unsigned long base, flash_info_t *info)
{

	short i;
	volatile unsigned char *flash8 = (volatile unsigned char *)base;
	volatile unsigned short *flash16 = (volatile unsigned short *)base;
	unsigned short man_id, dev_id;


#if 0
	/* sorry, I couldn't figure out what this should be good for ... */
	addr8[ADDR0] = (FLASH_CTRL_WORD_SIZE)0x00AA00AA;
	addr8[ADDR1] = (FLASH_CTRL_WORD_SIZE)0x00550055;
	addr8[0] = 0x60;   /* unprotecting flash ... ?? */
	addr8[0] = 0xd0;
	addr8[0] = 0x70;

	printf ("Status is %02x\n", addr8[0]);
	addr8[0] = 0x50;
	printf ("Status (after clear) is %02x\n", addr8[0]);
#endif



	if (base==0) {
		printf ("Error: base is 0!");
		return 0;
	}
	if (info==NULL) {
		printf ("Error: info is 0!");
		return 0;
	}

	/* Write auto select command: read Manufacturer ID */
	flash8[ADDR0] = (FLASH_CTRL_WORD_SIZE)0x00AA00AA;
	flash8[ADDR1] = (FLASH_CTRL_WORD_SIZE)0x00550055;
	flash8[ADDR0] = (FLASH_CTRL_WORD_SIZE)0x00900090;
	man_id = SWAP16(flash16[0]);
	dev_id = SWAP16(flash16[1]);
	printf ("   Flash bank at %08lx\n", base);
	printf ("      Manufacturer ID: %04x Device ID: %04x\n", man_id, dev_id);

	switch (man_id) {
		case (FLASH_WORD_SIZE)AMD_MANUFACT:
			info->flash_id = FLASH_MAN_AMD;
			break;
		case (FLASH_WORD_SIZE)FUJ_MANUFACT:
			info->flash_id = FLASH_MAN_FUJ;
			break;
		case (FLASH_WORD_SIZE)SST_MANUFACT:
			info->flash_id = FLASH_MAN_SST;
			break;
		case (FLASH_WORD_SIZE)STM_MANUFACT:
			info->flash_id = FLASH_MAN_STM;
			break;
		case (FLASH_WORD_SIZE)INTEL_MANUFACT:
			info->flash_id = FLASH_MAN_INTEL;
			break;
		default:
			info->flash_id = FLASH_UNKNOWN;
			info->sector_count = 0;
			info->size = 0;
			return 0;			/* no or unknown flash	*/
	}

	switch (dev_id) {
		case (FLASH_WORD_SIZE)AMD_ID_F040B:
			info->flash_id += FLASH_AM040;
			info->sector_count = 8;
			info->size = 0x0080000; /* => 512 ko */
			break;
		case (FLASH_WORD_SIZE)AMD_ID_LV400T:
			info->flash_id += FLASH_AM400T;
			info->sector_count = 11;
			info->size = 0x00080000;
			break;				/* => 0.5 MB		*/

		case (FLASH_WORD_SIZE)AMD_ID_LV400B:
			info->flash_id += FLASH_AM400B;
			info->sector_count = 11;
			info->size = 0x00080000;
			break;				/* => 0.5 MB		*/

		case (FLASH_WORD_SIZE)AMD_ID_LV800T:
			info->flash_id += FLASH_AM800T;
			info->sector_count = 19;
			info->size = 0x00100000;
			break;				/* => 1 MB		*/

		case (FLASH_WORD_SIZE)AMD_ID_LV800B:
			info->flash_id += FLASH_AM800B;
			info->sector_count = 19;
			info->size = 0x00100000;
			break;				/* => 1 MB		*/

		case (FLASH_WORD_SIZE)AMD_ID_LV160T:
			info->flash_id += FLASH_AM160T;
			info->sector_count = 35;
			info->size = 0x00200000;
			break;				/* => 2 MB		*/

		case (FLASH_WORD_SIZE)AMD_ID_LV160B:
			info->flash_id += FLASH_AM160B;
			info->sector_count = 35;
			info->size = 0x00200000;
			break;				/* => 2 MB		*/

		case (FLASH_WORD_SIZE)STM_ID_320T:
			info->flash_id += FLASH_AM320T;
			info->sector_count = 67;
			info->size = 0x00400000;
			break;				/* => 4 MB		*/

		case (FLASH_WORD_SIZE)STM_ID_320B:
			info->flash_id += FLASH_AM320B;
			info->sector_count = 67;
			info->size = 0x00400000;
			break;				/* => 4 MB		*/

		case (FLASH_WORD_SIZE)STM_ID_640T:
			info->flash_id += FLASH_ST640T;
			info->sector_count = 135;
			info->size = 0x00800000;
			break;				/* => 8 MB		*/

		case (FLASH_WORD_SIZE)STM_ID_640B:
			info->flash_id += FLASH_ST640B;
			info->sector_count = 135;
			info->size = 0x00800000;
			break;				/* => 8 MB		*/

		case (FLASH_WORD_SIZE)SST_ID_xF800A:
			info->flash_id += FLASH_SST800A;
			info->sector_count = 16;
			info->size = 0x00100000;
			break;				/* => 1 MB		*/

		case (FLASH_WORD_SIZE)SST_ID_xF160A:
			info->flash_id += FLASH_SST160A;
			info->sector_count = 32;
			info->size = 0x00200000;
			break;				/* => 2 MB		*/

		case (FLASH_WORD_SIZE)INTEL_ID_28F320J3A:
			info->flash_id += FLASH_28F320J3A;
			info->sector_count = 32;  /* of 128 KB size */
			info->size = 0x00400000;
			break;				/* => 4 MB		*/

		case (FLASH_WORD_SIZE)INTEL_ID_28F320C3T:
			info->flash_id += FLASH_28F320C3T;
			info->sector_count = 71;
			info->size = 0x00400000;
			break;				/* => 4 MB		*/

		case (FLASH_WORD_SIZE)INTEL_ID_28F640J3A:
			info->flash_id += FLASH_28F640J3A;
			info->sector_count = 64;  /* of 128 KB size */
			info->size = 0x00800000;
			break;				/* => 8 MB		*/

		case (FLASH_WORD_SIZE)INTEL_ID_28F128J3A:
			info->flash_id += FLASH_28F128J3A;
			info->sector_count = 128;  /* of 128 KB size */
			info->size = 0x01000000;
			break;				/* => 16 MB		*/

		default:
			info->flash_id = FLASH_UNKNOWN;
			return 0;			/* => no or unknown flash */

	}

	/* set up sector start address table */

	if ((info->flash_id & FLASH_TYPEMASK) == FLASH_28F320C3T) {
		unsigned long addr = base;
		volatile unsigned char *flash = 0;
		for (i=0; i<info->sector_count-8; i++) {
			info->start[i] = addr;

			/* Un-protect the Intel flashes .. */
			flash[addr] = 0x60;
			flash[addr] = 0xd0;
			flash[addr] = 0xff;
			info->protect[i] = 0;

			addr+=0x10000;
		}
		for (; i<info->sector_count; i++) {
			info->start[i] = addr;

			/* Un-protect the Intel flashes .. */
			flash[addr] = 0x60;
			flash[addr] = 0xd0;
			flash[addr] = 0xff;
			info->protect[i] = 0;

			addr+=0x2000;
		}
		return (info->size);
	}

	if (((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_SST) ||
			(info->flash_id  == FLASH_AM040)) {
		for (i = 0; i < info->sector_count; i++)
			info->start[i] = base + (i * 0x00010000);
	} else if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_INTEL) {
		for (i = 0; i < info->sector_count; i++)
			info->start[i] = base + (i * 0x00020000);
	} else if ((info->flash_id & FLASH_TYPEMASK) == FLASH_ST640B) {
		for (i = 0; i < 8; i++)
			info->start[i] = base + (i * 0x00002000);
		for ( ; i < info->sector_count; i++)
			info->start[i] = base + (i * 0x00010000) - 0x00070000;
	} else if ((info->flash_id & FLASH_TYPEMASK) == FLASH_ST640T) {
		for (i = 0; i < info->sector_count - 8; i++)
			info->start[i] = base + (i * 0x00010000);
		for ( ; i < info->sector_count; i++)
			info->start[i] = base + info->size - ((info->sector_count - i) * 0x00002000);
	} else {
		if (info->flash_id & FLASH_BTYPE) {
			/* set sector offsets for bottom boot block type	*/
			info->start[0] = base + 0x00000000;
			info->start[1] = base + 0x00004000;
			info->start[2] = base + 0x00006000;
			info->start[3] = base + 0x00008000;
			for (i = 4; i < info->sector_count; i++) {
				info->start[i] = base + (i * 0x00010000) - 0x00030000;
			}
		} else {
			/* set sector offsets for top boot block type		*/
			i = info->sector_count - 1;
			info->start[i--] = base + info->size - 0x00004000;
			info->start[i--] = base + info->size - 0x00006000;
			info->start[i--] = base + info->size - 0x00008000;
			for (; i >= 0; i--) {
				info->start[i] = base + i * 0x00010000;
			}
		}
	}

	/* read sector protection at sector address, (A7 .. A0) = 0x02 */
	/* check for protected sectors */
	for (i = 0; i < info->sector_count; i++) {
		if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_SST) {
			info->protect[i] = 0;
		} else {
			flash16 = (volatile unsigned short *) info->start[i];
			info->protect[i] = SWAP16 (flash16[2]) & 1;
		}
	}

	/*
	 * Reset to Read mode
	 */
	if (info->flash_id != FLASH_UNKNOWN) {
		flash8[0] = 0xf0;
	}

	return (info->size);
}

/*
 * The following code run from FLASH!
 * returned the start addr
 */
ulong get_env_ptr_f (void)
{

	short i;
	volatile unsigned char *flash8 = (volatile unsigned char *)FLASH_BASE1_PRELIM;
	volatile unsigned short *flash16 = (volatile unsigned short *)FLASH_BASE1_PRELIM;
	unsigned short dev_id;
	unsigned short	sector_count;	/* number of erase units		*/
	unsigned long size;
   	unsigned long   addr = FLASH_BASE0_PRELIM;

	if (CFG_MAX_FLASH_BANKS == 2) {

		/* Write auto select command: read Manufacturer ID */
		flash8[ADDR0] = (FLASH_CTRL_WORD_SIZE)0x00AA00AA;
		flash8[ADDR1] = (FLASH_CTRL_WORD_SIZE)0x00550055;
		flash8[ADDR0] = (FLASH_CTRL_WORD_SIZE)0x00900090;
		dev_id = SWAP16(flash16[1]);
	
		switch (dev_id) {
			case (FLASH_WORD_SIZE)AMD_ID_F040B:
				sector_count = 8;
				size = 0x0080000; /* => 512 ko */
				break;
			case (FLASH_WORD_SIZE)AMD_ID_LV400T:
				sector_count = 11;
				size = 0x00080000;
				break;				/* => 0.5 MB		*/
	
			case (FLASH_WORD_SIZE)AMD_ID_LV400B:
				sector_count = 11;
				size = 0x00080000;
				break;				/* => 0.5 MB		*/
	
			case (FLASH_WORD_SIZE)AMD_ID_LV800T:
				sector_count = 19;
				size = 0x00100000;
				break;				/* => 1 MB		*/
	
			case (FLASH_WORD_SIZE)AMD_ID_LV800B:
				sector_count = 19;
				size = 0x00100000;
				break;				/* => 1 MB		*/
	
			case (FLASH_WORD_SIZE)AMD_ID_LV160T:
				sector_count = 35;
				size = 0x00200000;
				break;				/* => 2 MB		*/
	
			case (FLASH_WORD_SIZE)AMD_ID_LV160B:
				sector_count = 35;
				size = 0x00200000;
				break;				/* => 2 MB		*/
	
			case (FLASH_WORD_SIZE)STM_ID_320T:
				sector_count = 67;
				size = 0x00400000;
				break;				/* => 4 MB		*/
	
			case (FLASH_WORD_SIZE)STM_ID_320B:
				sector_count = 67;
				size = 0x00400000;
				break;				/* => 4 MB		*/
	
			case (FLASH_WORD_SIZE)STM_ID_640T:
				sector_count = 135;
				size = 0x00800000;
				break;				/* => 8 MB		*/
	
			case (FLASH_WORD_SIZE)STM_ID_640B:
				sector_count = 135;
				size = 0x00800000;
				break;				/* => 8 MB		*/
	
			case (FLASH_WORD_SIZE)SST_ID_xF800A:
				sector_count = 16;
				size = 0x00100000;
				break;				/* => 1 MB		*/
	
			case (FLASH_WORD_SIZE)SST_ID_xF160A:
				sector_count = 32;
				size = 0x00200000;
				break;				/* => 2 MB		*/
	
			case (FLASH_WORD_SIZE)INTEL_ID_28F320J3A:
				sector_count = 32;  /* of 128 KB size */
				size = 0x00400000;
				break;				/* => 4 MB		*/
	
			case (FLASH_WORD_SIZE)INTEL_ID_28F320C3T:
				sector_count = 71;
				size = 0x00400000;
				break;				/* => 4 MB		*/
	
			case (FLASH_WORD_SIZE)INTEL_ID_28F640J3A:
				sector_count = 64;  /* of 128 KB size */
				size = 0x00800000;
				break;				/* => 8 MB		*/
	
			case (FLASH_WORD_SIZE)INTEL_ID_28F128J3A:
				sector_count = 128;  /* of 128 KB size */
				size = 0x01000000;
				break;				/* => 16 MB		*/
	
			default:
				return 0;			/* => no or unknown flash */
	
		}


		/* set up sector start address table */
		if (dev_id == (FLASH_WORD_SIZE)INTEL_ID_28F320C3T ) {
			addr = FLASH_BASE1_PRELIM ;
			for (i=0; i<sector_count-8; i++) {
				addr+=0x10000;
			}
			for (; i<sector_count-1; i++) {
				addr+=0x2000;
			}
		}
		else {
			if ((FLASH_BASE1_PRELIM - size) != FLASH_BASE0_PRELIM) {
				addr=FLASH_BASE1_PRELIM - size;
			}
		}
	}
	return (addr);
}

int unprotect_flashes (void)
	/* Unprotects hardware of Intel flashes - these flashes are
           protected after each reset, hence we need to unprotect them
           after every reset. Safe to call before flash_init () */
{
    int bank, i;

    for (bank=0;bank<CFG_MAX_FLASH_BANKS; bank++) {
        if ((flash_info[bank].flash_id & FLASH_TYPEMASK) == FLASH_28F320C3T) {
            volatile unsigned char *flash = 0;
	    unsigned long addr;

            for (i=0; i<flash_info[bank].sector_count; i++) {
                addr = flash_info[bank].start[i];

/* Un-protect the Intel flashes .. */
                flash[addr] = 0x60;
                flash[addr] = 0xd0;
                flash[addr] = 0xff;
   /* Leave the software protection alone .. */
            }
         }
    }
    return 0;
}

static int wait_for_flash (unsigned long polling_addr, int timeout)
  /* Test Flash erase/programming progress by looking at DQ6 which
     keeps toggling while controller is busy. Also test for DQ5 which
     signals if there was an error. The function is smart enough not
     to confuse a '1' bit on DQ5 with data just written by testing
     the toggle bit.
   */
{
        volatile unsigned char *flash = (volatile unsigned char*) polling_addr;
        int DQ6;
        int x;
	unsigned long start;

	start = get_timer (0);
        DQ6=flash[0] & 0x40;

        while (1) {
                x=flash[0];
                if ((x&0x40) == DQ6) return 0;
                if ((x&0x20) == 0x20) {
                        DQ6=flash[0] & 0x40;
                        if ((flash[0] & 0x40) == DQ6) {
                                return 0;
                        }
                        printf ("Flash error\n");
                        return -1;
                }
                DQ6 = x & 0x40;
        	if (get_timer(start) > timeout) {
			printf ("Timeout\n");
			return -1;
		}
        }

        return 0;
}

static int wait_for_intel_flash (unsigned long polling_addr, int wait_erase)
{
        volatile unsigned char *flash = (volatile unsigned char*) polling_addr;
	int error_bit = (wait_erase ? 0x20 : 0x10);
	int timeout = (wait_erase ? 100000000 : 1000000);

        while (((flash[0] & 0x80) != 0x80) && --timeout) ;
     	if (!timeout) {
		printf ("Timeout\n");
             	flash [0] = 0xff;  /* Returns to read array mode */

       		return 1;
	}
	if (flash [0] & error_bit) {
        	flash [0] = 0x50;  /* Clears error bits */
		return 4;  /* most likely protected .. */
       	}
	flash [0] = 0xff;  /* Returns to read array mode */

	return 0;
}

int wait_for_DQ7(flash_info_t *info, int sect)
{
	ulong start, now, last;
	volatile FLASH_CTRL_WORD_SIZE *addr = (FLASH_CTRL_WORD_SIZE *)(info->start[sect]);

	start = get_timer (0);
    last  = start;
    while ((addr[0] & (FLASH_CTRL_WORD_SIZE)0x00800080) != (FLASH_CTRL_WORD_SIZE)0x00800080) {
        if ((now = get_timer(start)) > CFG_FLASH_ERASE_TOUT) {
            printf ("Timeout\n");
            return -1;
        }
        /* show that we're waiting */
        if ((now - last) > 1000) {  /* every second */
            putc ('.');
            last = now;
        }
    }
	return 0;
}

/*-----------------------------------------------------------------------
 */

int flash_erase (flash_info_t *info, int s_first, int s_last)
{
	volatile FLASH_CTRL_WORD_SIZE *addr = (FLASH_CTRL_WORD_SIZE *)(info->start[0]);
	volatile FLASH_CTRL_WORD_SIZE *addr2;
	int flag, prot, sect;
	int i;

	if ((s_first < 0) || (s_first > s_last)) {
		if (info->flash_id == FLASH_UNKNOWN) {
			printf ("- missing\n");
		} else {
			printf ("- no sectors to erase\n");
		}
		return 1;;
	}

	if (info->flash_id == FLASH_UNKNOWN) {
		printf ("Can't erase unknown flash type - aborted\n");
		return 1;
	}

	prot = 0;
	for (sect=s_first; sect<=s_last; ++sect) {
		if (info->protect[sect]) {
			prot++;
		}
	}

	if (prot) {
		printf ("- Warning: %d protected sectors will not be erased!\n",
			prot);
	} else {
		printf ("\n");
	}

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts();

	/* Start erase on unprotected sectors */
	for (sect = s_first; sect<=s_last; sect++) {
		addr2 = (FLASH_CTRL_WORD_SIZE *)(info->start[sect]);

		if (info->protect[sect] != 0) {
			printf ("Not erasing protected sector %p\n", addr2);
			continue;
		}

		printf("Erasing sector %p\n", addr2);	// CLH

		if ((info->flash_id & FLASH_TYPEMASK) == FLASH_28F320C3T) {
			int ret;
			addr[0] = 0x20;
			addr2[0] = 0xd0;
			if ((ret=wait_for_intel_flash (info->start[0], 1))!=0) {
				if (flag) enable_interrupts();
				return 1;
			}
			continue;
		}
		addr[ADDR0] = (FLASH_CTRL_WORD_SIZE)0x00AA00AA;
		addr[ADDR1] = (FLASH_CTRL_WORD_SIZE)0x00550055;
		addr[ADDR0] = (FLASH_CTRL_WORD_SIZE)0x00800080;
		addr[ADDR0] = (FLASH_CTRL_WORD_SIZE)0x00AA00AA;
		addr[ADDR1] = (FLASH_CTRL_WORD_SIZE)0x00550055;

		if ((info->flash_id & FLASH_VENDMASK) == FLASH_MAN_SST) {
			addr2[0] = (FLASH_CTRL_WORD_SIZE)0x00500050;
				/* block erase */
			for (i=0; i<50; i++) {
				udelay(1000);
			}
  /* Unclear .. SST flashes seem not to have the DQ6 toggle mechanism .. */
		} else {
			addr2[0] = (FLASH_CTRL_WORD_SIZE)0x00300030;  /* sector erase */
			if (wait_for_flash(info->start[0], CFG_FLASH_ERASE_TOUT)<0) {
				printf ("Aborting erasure.\n");
				if (flag) enable_interrupts();
				return 1;
			}
		}
	}

	/* re-enable interrupts if necessary */
	if (flag) enable_interrupts();

	/* wait at least 80us - let's wait 1 ms */
	udelay (1000);

/* ?? not neccessary, but it doesn't hurt .. */
	/* reset to read mode */
	addr = (FLASH_CTRL_WORD_SIZE *)info->start[0];
	addr[0] = (FLASH_CTRL_WORD_SIZE)0x00F000F0;	/* reset bank */

	printf ("Done\n");
	return 0;
}

/*-----------------------------------------------------------------------
 * Copy memory to flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 */

int write_buff (flash_info_t *info, uchar *src, ulong addr, ulong cnt)
{
	ulong cp, wp, data;
	int i, l, rc;

	wp = (addr & ~3);	/* get lower word aligned address */

	/*
	 * handle unaligned start bytes
	 */
	if ((l = addr - wp) != 0) {
		data = 0;
		for (i=0, cp=wp; i<l; ++i, ++cp) {
			data = (data << 8) | (*(uchar *)cp);
		}
		for (; i<4 && cnt>0; ++i) {
			data = (data << 8) | *src++;
			--cnt;
			++cp;
		}
		for (; cnt==0 && i<4; ++i, ++cp) {
			data = (data << 8) | (*(uchar *)cp);
		}

		if ((rc = write_word(info, wp, data)) != 0) {
			return (rc);
		}
		wp += 4;
	}

	/*
	 * handle word aligned part
	 */
	while (cnt >= 4) {
		data = 0;
		for (i=0; i<4; ++i) {
			data = (data << 8) | *src++;
		}
		if ((rc = write_word(info, wp, data)) != 0) {
			return (rc);
		}
		wp  += 4;
		cnt -= 4;
	}

	if (cnt == 0) {
		return (0);
	}

	/*
	 * handle unaligned tail bytes
	 */
	data = 0;
	for (i=0, cp=wp; i<4 && cnt>0; ++i, ++cp) {
		data = (data << 8) | *src++;
		--cnt;
	}
	for (; i<4; ++i, ++cp) {
		data = (data << 8) | (*(uchar *)cp);
	}

	return (write_word(info, wp, data));
}

/*-----------------------------------------------------------------------
 * Write a 32-bit word to Flash, returns:
 * 0 - OK
 * 1 - write timeout
 * 2 - Flash not erased
 *
 * dest must be 32-bit aligned !!
 */

/* Note: On TC16.1, data lines are attached byte reversed. Nevertheless
   we are not byte-reversing when writing, because we'd have to byte
   reverse back on reading ... */


static int write_word (flash_info_t *info, unsigned long dest, unsigned long data)
{
	volatile FLASH_CTRL_WORD_SIZE *addr2 = (FLASH_CTRL_WORD_SIZE *)(info->start[0]);
        volatile FLASH_WORD_SIZE *dest2 = (FLASH_WORD_SIZE *)dest;
        volatile FLASH_WORD_SIZE *data2 = (FLASH_WORD_SIZE *)&data;
	int flag;
        int i;

	/* Check if Flash is (sufficiently) erased */
	if ((*((volatile FLASH_WORD_SIZE *)dest) &
             (FLASH_WORD_SIZE)data) != (FLASH_WORD_SIZE)data) {
		return (2);
	}

	/* Disable interrupts which might cause a timeout here */
	flag = disable_interrupts();

        for (i=0; i<4/sizeof(FLASH_WORD_SIZE); i++)
	{
	        if ((info->flash_id & FLASH_TYPEMASK) == FLASH_28F320C3T) {
			addr2[0] = 0x40;
		} else {
	 		addr2[ADDR0] = (FLASH_CTRL_WORD_SIZE)0x00AA00AA;
			addr2[ADDR1] = (FLASH_CTRL_WORD_SIZE)0x00550055;
			addr2[ADDR0] = (FLASH_CTRL_WORD_SIZE)0x00A000A0;
		}

		dest2[i] = data2[i];

		if ((info->flash_id & FLASH_TYPEMASK) == FLASH_28F320C3T) {
			int ret;
			if ((ret=wait_for_intel_flash (info->start[0], 0))!=0) {
				if (flag) enable_interrupts();
				return ret;
			}
		} else {
			if (wait_for_flash(info->start[0], CFG_FLASH_WRITE_TOUT)<0) {
				if (flag) enable_interrupts();
				return 1;
			}
		}
	}
	if (flag) enable_interrupts();

	return (0);
}

