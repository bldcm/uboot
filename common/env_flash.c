/*
 * (C) Copyright 2000-2002
 * Wolfgang Denk, DENX Software Engineering, wd@denx.de.
 *
 * (C) Copyright 2001 Sysgo Real-Time Solutions, GmbH <www.elinos.com>
 * Andreas Heppel <aheppel@sysgo.de>

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

/* #define DEBUG */

#include <common.h>

#if defined(CFG_ENV_IS_IN_FLASH) /* Environment is in Flash */

#include <command.h>
#include <linux/stddef.h>
#include <malloc.h>


#include <environment.h>


#if ((CONFIG_COMMANDS&(CFG_CMD_ENV|CFG_CMD_FLASH)) == (CFG_CMD_ENV|CFG_CMD_FLASH))
#define CMD_SAVEENV
#elif defined(CFG_ENV_ADDR_REDUND)
#error Cannot use CFG_ENV_ADDR_REDUND without CFG_CMD_ENV & CFG_CMD_FLASH
#endif

#if defined(CFG_ENV_SIZE_REDUND) && (CFG_ENV_SIZE_REDUND < CFG_ENV_SIZE)
#error CFG_ENV_SIZE_REDUND should not be less then CFG_ENV_SIZE
#endif

#ifdef CONFIG_INFERNO
# ifdef CFG_ENV_ADDR_REDUND
#error CFG_ENV_ADDR_REDUND is not implemented for CONFIG_INFERNO
# endif
#endif

char * env_name_spec = "Flash";

#ifdef ENV_IS_EMBEDDED

extern uchar environment[];
env_t *env_ptr = (env_t *)(&environment[0]);

#ifdef CMD_SAVEENV
/* static env_t *flash_addr = (env_t *)(&environment[0]);-broken on ARM-wd-*/
static env_t *flash_addr = (env_t *)CFG_ENV_ADDR;
#endif

#else /* ! ENV_IS_EMBEDDED */

env_t *env_ptr = (env_t *)CFG_ENV_ADDR;
#ifdef CMD_SAVEENV
static env_t *flash_addr = (env_t *)CFG_ENV_ADDR;
#endif

#endif /* ENV_IS_EMBEDDED */

#ifdef CFG_ENV_ADDR_REDUND
static env_t *flash_addr_new = (env_t *)CFG_ENV_ADDR_REDUND;

/* CFG_ENV_ADDR is supposed to be on sector boundary */
static ulong end_addr = CFG_ENV_ADDR + CFG_ENV_SECT_SIZE - 1;
static ulong end_addr_new = CFG_ENV_ADDR_REDUND + CFG_ENV_SECT_SIZE - 1;

#define ACTIVE_FLAG   1
#define OBSOLETE_FLAG 0
#endif /* CFG_ENV_ADDR_REDUND */

extern uchar default_environment[];
extern int default_environment_size;


uchar env_get_char_spec (int index)
{
	DECLARE_GLOBAL_DATA_PTR;

	return ( *((uchar *)(gd->env_addr + index)) );
}

#ifdef CFG_ENV_ADDR_REDUND

int  env_init(void)
{
	DECLARE_GLOBAL_DATA_PTR;
	int crc1_ok = 0, crc2_ok = 0;

	uchar flag1 = flash_addr->flags;
	uchar flag2 = flash_addr_new->flags;

	ulong addr_default = (ulong)&default_environment[0];
	ulong addr1 = (ulong)&(flash_addr->data);
	ulong addr2 = (ulong)&(flash_addr_new->data);

#ifdef CONFIG_OMAP2420H4
	int flash_probe(void);

	if(flash_probe() == 0)
		goto bad_flash;
#endif

	crc1_ok = (crc32(0, flash_addr->data, ENV_SIZE) == flash_addr->crc);
	crc2_ok = (crc32(0, flash_addr_new->data, ENV_SIZE) == flash_addr_new->crc);

	if (crc1_ok && ! crc2_ok) {
		gd->env_addr  = addr1;
		gd->env_valid = 1;
	} else if (! crc1_ok && crc2_ok) {
		gd->env_addr  = addr2;
		gd->env_valid = 1;
	} else if (! crc1_ok && ! crc2_ok) {
		gd->env_addr  = addr_default;
		gd->env_valid = 0;
	} else if (flag1 == ACTIVE_FLAG && flag2 == OBSOLETE_FLAG) {
		gd->env_addr  = addr1;
		gd->env_valid = 1;
	} else if (flag1 == OBSOLETE_FLAG && flag2 == ACTIVE_FLAG) {
		gd->env_addr  = addr2;
		gd->env_valid = 1;
	} else if (flag1 == flag2) {
		gd->env_addr  = addr1;
		gd->env_valid = 2;
	} else if (flag1 == 0xFF) {
		gd->env_addr  = addr1;
		gd->env_valid = 2;
	} else if (flag2 == 0xFF) {
		gd->env_addr  = addr2;
		gd->env_valid = 2;
	}

#ifdef CONFIG_OMAP2420H4
bad_flash:
#endif
	return (0);
}

#ifdef CMD_SAVEENV
int saveenv(void)
{
	char *saved_data = NULL;
	int rc = 1;
	char flag = OBSOLETE_FLAG, new_flag = ACTIVE_FLAG;
#if CFG_ENV_SECT_SIZE > CFG_ENV_SIZE
	ulong up_data = 0;
#endif

	debug ("Protect off %08lX ... %08lX\n",
		(ulong)flash_addr, end_addr);

	if (flash_sect_protect (0, (ulong)flash_addr, end_addr)) {
		goto Done;
	}

	debug ("Protect off %08lX ... %08lX\n",
		(ulong)flash_addr_new, end_addr_new);

	if (flash_sect_protect (0, (ulong)flash_addr_new, end_addr_new)) {
		goto Done;
	}

#if CFG_ENV_SECT_SIZE > CFG_ENV_SIZE
	up_data = (end_addr_new + 1 - ((long)flash_addr_new + CFG_ENV_SIZE));
	debug ("Data to save 0x%x\n", up_data);
	if (up_data) {
		if ((saved_data = malloc(up_data)) == NULL) {
			printf("Unable to save the rest of sector (%ld)\n",
				up_data);
			goto Done;
		}
		memcpy(saved_data,
			(void *)((long)flash_addr_new + CFG_ENV_SIZE), up_data);
		debug ("Data (start 0x%x, len 0x%x) saved at 0x%x\n",
			   (long)flash_addr_new + CFG_ENV_SIZE,
				up_data, saved_data);
	}
#endif
	puts ("Erasing Flash...");
	debug (" %08lX ... %08lX ...",
		(ulong)flash_addr_new, end_addr_new);

	if (flash_sect_erase ((ulong)flash_addr_new, end_addr_new)) {
		goto Done;
	}

	puts ("Writing to Flash... ");
	debug (" %08lX ... %08lX ...",
		(ulong)&(flash_addr_new->data),
		sizeof(env_ptr->data)+(ulong)&(flash_addr_new->data));
	if ((rc = flash_write((char *)env_ptr->data,
			(ulong)&(flash_addr_new->data),
			sizeof(env_ptr->data))) ||
	    (rc = flash_write((char *)&(env_ptr->crc),
			(ulong)&(flash_addr_new->crc),
			sizeof(env_ptr->crc))) ||
	    (rc = flash_write(&flag,
			(ulong)&(flash_addr->flags),
			sizeof(flash_addr->flags))) ||
	    (rc = flash_write(&new_flag,
			(ulong)&(flash_addr_new->flags),
			sizeof(flash_addr_new->flags))))
	{
		flash_perror (rc);
		goto Done;
	}
	puts ("done\n");

#if CFG_ENV_SECT_SIZE > CFG_ENV_SIZE
	if (up_data) { /* restore the rest of sector */
		debug ("Restoring the rest of data to 0x%x len 0x%x\n",
			   (long)flash_addr_new + CFG_ENV_SIZE, up_data);
		if (flash_write(saved_data,
				(long)flash_addr_new + CFG_ENV_SIZE,
				up_data)) {
			flash_perror(rc);
			goto Done;
		}
	}
#endif
	{
		env_t * etmp = flash_addr;
		ulong ltmp = end_addr;

		flash_addr = flash_addr_new;
		flash_addr_new = etmp;

		end_addr = end_addr_new;
		end_addr_new = ltmp;
	}

	rc = 0;
Done:

	if (saved_data)
		free (saved_data);
	/* try to re-protect */
	(void) flash_sect_protect (1, (ulong)flash_addr, end_addr);
	(void) flash_sect_protect (1, (ulong)flash_addr_new, end_addr_new);

	return rc;
}
#endif /* CMD_SAVEENV */

#else /* ! CFG_ENV_ADDR_REDUND */
extern void serial0_puts(char *str);	

int  env_init(void)
{
	int i;
	env_t *loc_env_ptr;
	env_t *used_env_ptr=0;
	char str_version[7];
	int act_version=0;
	int next_version=0;
	int valid_env_found=0;
	char buffer[30];
	DECLARE_GLOBAL_DATA_PTR;
#ifdef CONFIG_OMAP2420H4
	int flash_probe(void);

	if(flash_probe() == 0)
		goto bad_flash;
#endif
#if CONFIG_TC18
#define NAME_ENV_TIMESTAMP "_timestamp"


	loc_env_ptr=env_ptr;
	for (i=0;i<CFG_ENV_NUMBER;i++){
		loc_env_ptr=(env_t*)((ulong)env_ptr+i*CFG_ENV_SIZE);
		if (crc32(0, loc_env_ptr->data,ENV_SIZE) == loc_env_ptr->crc) {
			if (gd->env_valid==0) {
				puts("Environment found in FLASH:\n");
				gd->env_valid = 1;
			}
			//show loc_env_ptr
			sprintf(buffer,"at 0x%08p ",loc_env_ptr);
			serial0_puts(buffer);	
			next_version=0;
			gd->env_addr  = (ulong)&(loc_env_ptr->data);
			if (used_env_ptr==0) {
				gd->env_flash_pos=0;
				used_env_ptr=loc_env_ptr;
			}
			if (getenv_r(NAME_ENV_TIMESTAMP,str_version,sizeof(str_version))>0) {
				puts(" - ");
				puts(str_version);
				putc('\n');
				next_version=(int)simple_strtoul (str_version, NULL, 10);
				switch(str_version[5]) {
				case 'P':
				case 'N': 
				case 'T':
				{
					valid_env_found=1;
					if ((i==0)||((next_version!=act_version)&&((((next_version-act_version)+10000)%10000)<5000))){
						act_version=next_version;
						used_env_ptr=loc_env_ptr;
						gd->env_flash_pos=i;
					} 
					break;
				}
				case 'F':
				default: 
					;
				}
			}
			else {
				puts(" - no timestamp\n");
				act_version=next_version;
				valid_env_found=1;
			}
		}
	}

	if (valid_env_found==1) {
		gd->env_addr=(ulong)&(used_env_ptr->data);
		sprintf(buffer,"Used environment from 0x%08p\n",used_env_ptr);
		serial0_puts(buffer);	
		return(0);
	}
	else {
		puts("No valid environment in FLASH!\n");
		gd->env_valid = 0;
	}

#else
	if (crc32(0, env_ptr->data, ENV_SIZE) == env_ptr->crc) {
		gd->env_addr  = (ulong)&(env_ptr->data);
		gd->env_valid = 1;
		return(0);
	}
#endif

#ifdef CONFIG_OMAP2420H4
bad_flash:
#endif
	gd->env_addr  = (ulong)&default_environment[0];
	gd->env_valid = 0;
	return (0);
}

#ifdef CMD_SAVEENV
int ext_saveenv(int pos,uchar ldelete)
{
	int	len, rc;
	ulong	end_addr;
	ulong	loc_flash_addr;
	ulong	flash_sect_addr;
#if defined(CFG_ENV_SECT_SIZE) && (CFG_ENV_SECT_SIZE > CFG_ENV_SIZE)
	ulong	flash_offset;
	uchar	env_buffer[CFG_ENV_SECT_SIZE];
 #else
	uchar *env_buffer = (uchar *)env_ptr;
#endif	/* CFG_ENV_SECT_SIZE */
	int rcode = 0;


#ifdef CONFIG_TC16 
	if (flash_info[0].flash_id == FLASH_UNKNOWN) {
		printf ("No or unknown flash, cannot save environment!\n");
		return 1;
	}
/* The Intel flashes are Top boot sector, so put environment in the 
   toppest boot sector of the top flash bank .. */

	if ((flash_info[1].flash_id & FLASH_TYPEMASK) == FLASH_28F320C3T) {
		flash_addr = (env_t *) flash_info[1].start[flash_info[1].sector_count-1];
	} else {
		flash_addr = (env_t *) flash_info[0].start[0];
	}
#endif

#if defined(CFG_ENV_SECT_SIZE) && (CFG_ENV_SECT_SIZE > CFG_ENV_SIZE)

	loc_flash_addr=(ulong)((ulong)flash_addr+pos*CFG_ENV_SIZE);
	
	flash_offset    = ((ulong)loc_flash_addr) & (CFG_ENV_SECT_SIZE-1);
	flash_sect_addr = ((ulong)loc_flash_addr) & ~(CFG_ENV_SECT_SIZE-1);
	if (ldelete) {
		debug ( "clear old content\n");
		memset (env_buffer, 0xff, CFG_ENV_SECT_SIZE); // clear env_buffer old environment will be remove
	}
	else {
		debug ( "copy old content: "
			"sect_addr: %08lX  env_addr: %08lX  offset: %08lX\n",
			flash_sect_addr, (ulong)flash_addr, flash_offset);
	
		/* copy old contents to temporary buffer */
		memcpy (env_buffer, (void *)flash_sect_addr, CFG_ENV_SECT_SIZE);
	}

	/* copy current environment to temporary buffer */
	memcpy ((uchar *)((unsigned long)env_buffer + flash_offset),
		env_ptr,
		CFG_ENV_SIZE);

	len	 = CFG_ENV_SECT_SIZE;
#else
	flash_sect_addr = (ulong)flash_addr;
	len	 = CFG_ENV_SIZE;
#endif	/* CFG_ENV_SECT_SIZE */

#ifndef CONFIG_INFERNO
	end_addr = flash_sect_addr + len - 1;
#else
	/* this is the last sector, and the size is hardcoded here */
	/* otherwise we will get stack problems on loading 128 KB environment */
	end_addr = flash_sect_addr + 0x20000 - 1;
#endif

	debug ("Protect off %08lX ... %08lX\n",
		(ulong)flash_sect_addr, end_addr);

	if (flash_sect_protect (0, flash_sect_addr, end_addr))
		return 1;

	puts ("Erasing Flash...");
	if (flash_sect_erase (flash_sect_addr, end_addr))
		return 1;

	puts ("Writing to Flash... ");
	rc = flash_write((char *)env_buffer, flash_sect_addr, len);
	if (rc != 0) {
		flash_perror (rc);
		rcode = 1;
	} else {
		puts ("done\n");
	}

	/* try to re-protect */
	(void) flash_sect_protect (1, flash_sect_addr, end_addr);
	return rcode;
}
int saveenv(void) {
	DECLARE_GLOBAL_DATA_PTR;
	return ext_saveenv(gd->env_flash_pos,0);
}
#endif /* CMD_SAVEENV */

#endif /* CFG_ENV_ADDR_REDUND */

void env_relocate_spec (void)
{
	DECLARE_GLOBAL_DATA_PTR;
#if !defined(ENV_IS_EMBEDDED) || defined(CFG_ENV_ADDR_REDUND)
#ifdef CFG_ENV_ADDR_REDUND

	if (gd->env_addr != (ulong)&(flash_addr->data)) {
		env_t * etmp = flash_addr;
		ulong ltmp = end_addr;

		flash_addr = flash_addr_new;
		flash_addr_new = etmp;

		end_addr = end_addr_new;
		end_addr_new = ltmp;
	}

	if (flash_addr_new->flags != OBSOLETE_FLAG &&
	    crc32(0, flash_addr_new->data, ENV_SIZE) ==
	    flash_addr_new->crc) {
		char flag = OBSOLETE_FLAG;

		gd->env_valid = 2;
		flash_sect_protect (0, (ulong)flash_addr_new, end_addr_new);
		flash_write(&flag,
			    (ulong)&(flash_addr_new->flags),
			    sizeof(flash_addr_new->flags));
		flash_sect_protect (1, (ulong)flash_addr_new, end_addr_new);
	}

	if (flash_addr->flags != ACTIVE_FLAG &&
	    (flash_addr->flags & ACTIVE_FLAG) == ACTIVE_FLAG) {
		char flag = ACTIVE_FLAG;

		gd->env_valid = 2;
		flash_sect_protect (0, (ulong)flash_addr, end_addr);
		flash_write(&flag,
			    (ulong)&(flash_addr->flags),
			    sizeof(flash_addr->flags));
		flash_sect_protect (1, (ulong)flash_addr, end_addr);
	}

	if (gd->env_valid == 2)
		puts ("*** Warning - some problems detected "
		      "reading environment; recovered successfully\n\n");
#endif /* CFG_ENV_ADDR_REDUND */
#if CONFIG_TC18
	memcpy (env_ptr, (void*)((uchar *)flash_addr+gd->env_flash_pos*CFG_ENV_SIZE), CFG_ENV_SIZE);
#else
	memcpy (env_ptr, (void*)flash_addr, CFG_ENV_SIZE);
#endif
#endif /* ! ENV_IS_EMBEDDED || CFG_ENV_ADDR_REDUND */
}

#endif /* CFG_ENV_IS_IN_FLASH */
