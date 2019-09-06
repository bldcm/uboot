/*-!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!
! H-Modul: LCD.H       Bez.: Definitionen fuer LCD     Teil von: ---    !
!                                                                       !
! Vers. Datum    Autor     Aenderungen                                  !
! ------------------------------------                                  !
! 3.7   16.12.97  JH       neu                                          !
!                                                                       !
!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!-*/

#ifndef __LCD_H
#define __LCD_H

/* Importmant note: check this, as well as serial.c, include/405*.h
   for endianness when switching I/O area to little endian. 
   Right now, everything is still configured for big endian. */

/************************************************/
/* Definitionen fuer LED's, IO RESET  und SOUND */
/************************************************/
#define GREENLED  BIT6
#define REDLED    BIT7

/*********************************/
/* Definitionen fuer LCD-Display */
/*********************************/

/* LCD is using GPIO pins 9 - 14 (Note: 0 is MSB since I/O area is 
                                  still big-endian.) */
 
#define LCD_MASK 0xff81ffff


/*************************************************************/
/* register select */
#define  INST       (unsigned long) 0           /* Instruction register      */
#define  DATA       (unsigned long) 0x00040000  /* Daten register (GPIO13)   */

#define  DSBL       (unsigned long) 0           /* Disable                   */
#define  ENBL       (unsigned long) 0x00020000  /* Enable (GPIO14)           */
                                /*                           */
#define  ABEG2      0x40        /* Startadresse der 2. Zeile */
#define  LEN1       16          /* Zeichen pro Zeile         */
#define  LEN2       32          /* Max. Anzahl Zeichen       */
/*************************************************************/


/************************************************/
/* Funktionen                                   */
/************************************************/
int  lcd_init (void);
int  lcd_clear (void);
void  lcd_print(unsigned long start, char *s);

#define BUTTON_MODE_MASK 0x00000040   /* GPIO 25 */
#define BUTTON_SET_MASK 0x00000020    /* GPIO 26 */

/* Bit 0: SW1/Mode pressed. 
   Bit 1: SW2/Set pressed. */

#define BUTTON_MODE 1
#define BUTTON_SET  2

int push_button (void);
#endif

