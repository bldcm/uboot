/* Derived from: lcd.c by JH */

#include "lcd.h"
//#include "gpio.h"
#include <common.h>
#include <command.h>


/* Reverses bit order ... */

static const unsigned long lcd_values[16] = {
  0x00000000, 0x00000001, 0x00000002, 0x00000003,
  0x00000004, 0x00000005, 0x00000006, 0x00000007,
  0x00000008, 0x00000009, 0x0000000a, 0x0000000b,
  0x0000000c, 0x0000000d, 0x0000000e, 0x0000000f};




/* approximate delay .. busy wait, do not use in Linux kernel. */
static void delay (int ms)
{
   udelay(ms*1000);

/* int i=0;
        while (ms--) {
                for (i=0; i<110000; i++) ;
        }*/
}


static void outnib (unsigned long nib, unsigned long inst)

      /* Gibt die unteren 4 Bit von 'nib' an das LCD Display aus. */

{
  volatile unsigned long *gpio_out0=(unsigned long *) GPIO0_OR;
  volatile unsigned long *gpio_out1=(unsigned long *) GPIO1_OR;

  nib &= 0x0f;

  *gpio_out1 = (*gpio_out1 & LCD_MASK1) | DSBL | inst ;
   delay(1);
  *gpio_out0 = (*gpio_out0 & LCD_MASK0) | nib  ;
  *gpio_out1 = (*gpio_out1 & LCD_MASK1) | ENBL | inst ;
  delay(1);
  *gpio_out1 = (*gpio_out1 & LCD_MASK1) | DSBL | inst ;
   delay(1);
}


static void out (unsigned long byte, unsigned long inst)

     /* Schreibt 'byte' in Register 'inst' des LCD Displays */

{
  outnib((byte >> 4) & 0xf,inst);
  outnib(byte & 0xf,inst);
}


void lcd_print (unsigned long start,char *s)
{
  if (start < LEN1) {
    /* 1. Zeile */
    out(start | 0x80,INST);   /* Cursor in 1. Zeile setzen */

    while (start < LEN1 && *s != 0) {
      delay(1);
      out(*s,DATA);
      start++;
      s++;
    }
  }
  if (start < LEN2 && start >= LEN1) {
    /* 2. Zeile */
    delay(1);
    out((ABEG2 - LEN1 + start) | 0x80,INST); /* Cursor in 2. Zeile */
    while (start < LEN2 && *s != 0) {
      delay(1);
      out(*s,DATA);
      start++;
      s++;
    }
  }
}

int lcd_test (void)
{
lcd_print (0,"LCD Test OK !");
return 0;
}


int lcd_init (void)
      /* Initialisiert das LCD Display nach Power-On. */

{
 
  outnib(0x03,INST);
  delay(4);
  outnib(0x03,INST);
  delay(1);
  outnib(0x03,INST);
  delay(1);

  outnib(0x02,INST);  /* 4 Bit-Bus  mode */
  delay(1);
  out(0x28,INST);     /* 4 Bit-Bus, 2-Zeilig, 5x7 dots */
  delay(1);
  out(0x08,INST);     /* Display off */
  delay(1);
  out(0x01,INST);     /* Clear Display */
  delay(1);
  out(0x06,INST);     /* Cursor Increment mode */
  delay(1);
  out(0x0c,INST);     /* Display on, Cursor off, Blink off */
  delay(1);
  return 0;	
}

int lcd_clear (void)
      /* Clear LCD */

{
  lcd_print (0, "\xff\xff\xff\xff\xff\xff\xff\xff" \
		      "\xff\xff\xff\xff\xff\xff\xff\xff" \
		      "\xff\xff\xff\xff\xff\xff\xff\xff" \
		      "\xff\xff\xff\xff\xff\xff\xff\xff");
  return 0;	
}



int push_button (void)
{
        volatile unsigned long *gpio_in=(unsigned long *) (GPIO0_IR);
        int ret=0;
        unsigned long sample;

        sample = *gpio_in;

        if (sample & BUTTON_MODE_MASK) ret+=BUTTON_MODE;
        if (sample & BUTTON_SET_MASK) ret+=BUTTON_SET;

        return ret;
}


int do_lcdprint (cmd_tbl_t *cmdtp, int flag, int argc, char *argv[])
{
        int i;
        int pos=0;

/*	for (i = 0; i < 63; i++) {
		debug("TLB[%i] = %8x %8x %8x\n", i, ReadTlb0(i), ReadTlb1(i), ReadTlb2(i));
	}*/

if (argc==1) {
printf ("Initialising LCD.\n");
        lcd_init ();
        return 0;
}
        for (i=1;i<argc;++i) {
printf ("printing %s at position %d.\n", argv[i], pos);
                lcd_print (pos, argv[i]);
                pos+=strlen (argv[i]);
                lcd_print (pos, " ");
                pos++;
        }
  	return 0;
}

#if (CONFIG_COMMANDS & CFG_CMD_BSP)

U_BOOT_CMD(
	lcd,	CFG_MAXARGS,	0,	do_lcdprint,
	"lcd     - print string to lcd.\n",""\
);


U_BOOT_CMD(
	pci_init,	CFG_MAXARGS,	0,	pci_init,
	"pci_init    - call pci_init.\n",""\
);

#endif


