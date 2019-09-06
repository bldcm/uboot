/*-!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!--!
! H-Modul: LCD.H       Bez.: Definitionen fuer LCD     Teil von: ---    !
!                                                                       !
! Vers. Datum    Autor     Aenderungen                                  !
! ------------------------------------                                  !
! 3.7   16.12.97  JH       neu                                          !
!       05.04.06  Isko     changes for TC18                             !
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

/* LCD is using GPIO pins 28 - 33 (Note: 0 is MSB since I/O area is 
                                  still big-endian.) */
 
#define LCD_MASK0 0xfffffff0 /* GPIO pin 28-31 */
#define LCD_MASK1 0x3fffffff /* GPIO pin 32-33 */


/*************************************************************/
/* register select */
#define  INST       (unsigned long) 0           /* Instruction register      */
#define  DATA       (unsigned long) 0x80000000  /* Daten register (GPIO32)   */

#define  DSBL       (unsigned long) 0		 /* Disable                   */
#define  ENBL       (unsigned long) 0x40000000  /* Enable (GPIO33)           */
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

#define BUTTON_MODE_MASK 0x00000010   /* GPIO 27 */
#define BUTTON_SET_MASK 0x00000020    /* GPIO 26 */

/* Bit 0: SW1/Mode pressed. 
   Bit 1: SW2/Set pressed. */

#define BUTTON_MODE 2
#define BUTTON_SET  1

int push_button (void);
#endif

