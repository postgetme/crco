/*----------------------------------------------------------------------------
 *      RL-ARM - FlashFS
 *----------------------------------------------------------------------------
 *      Name:    SPI_LM3S37x8.c
 *      Purpose: Serial Peripheral Interface Driver for Luminary LM3S37x8
 *      Rev.:    V4.13
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2010 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <File_Config.h>
#include <LM3Sxxxx.H>

#ifdef SSI1
 #define SSIx_PERI      SYSCTL_PERIPH_SSI1
 #define SSIx_BASE      SSI1_BASE
#else
 #define SSIx_PERI      SYSCTL_PERIPH_SSI0
 #define SSIx_BASE      SSI0_BASE
#endif

/* Peripheral definitions for EK-LM3S37x8 board */
#define SSI_PORT_PERI   SYSCTL_PERIPH_GPIOA
#define CS_PORT_PERI    SYSCTL_PERIPH_GPIOA
#define SSI_PORT        GPIO_PORTA_BASE
#define CS_PORT         GPIO_PORTA_BASE

#define SSI_CS          GPIO_PIN_3
#define SSI_CLK         GPIO_PIN_2
#define SSI_RX          GPIO_PIN_4
#define SSI_TX          GPIO_PIN_5
#define SSI_PINS        (SSI_TX | SSI_RX | SSI_CLK)


/*----------------------------------------------------------------------------
 *      SPI Driver Functions
 *----------------------------------------------------------------------------
 *  Required functions for SPI driver module:
 *   - void spi_init     ()
 *   - void spi_ss       (U32 ss)
 *   - U8   spi_send     (U8 outb)
 *   - void spi_hi_speed (BOOL on)
 *---------------------------------------------------------------------------*/

/*--------------------------- spi_init --------------------------------------*/

void spi_init (void) {
  /* Initialize and enable the SSP Interface module. */

  /* Enable the SSI peripherals. */
  SysCtlPeripheralEnable(SSIx_PERI);
  SysCtlPeripheralEnable(SSI_PORT_PERI);
  SysCtlPeripheralEnable(CS_PORT_PERI);

  /* Configure the appropriate pins to be SSI instead of GPIO */
  GPIODirModeSet(SSI_PORT, SSI_PINS, GPIO_DIR_MODE_HW);
  GPIODirModeSet(CS_PORT,  SSI_CS,   GPIO_DIR_MODE_OUT);
  GPIOPadConfigSet(SSI_PORT, SSI_PINS, GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);
  GPIOPadConfigSet(CS_PORT,  SSI_CS,   GPIO_STRENGTH_4MA, GPIO_PIN_TYPE_STD_WPU);

  /* Deassert the SSIx chip select */
  GPIOPinWrite(CS_PORT, SSI_CS, SSI_CS);

  /* Configure the SSIx port */
  SSIConfig(SSIx_BASE, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, 400000, 8);
  SSIEnable(SSIx_BASE);
}


/*--------------------------- spi_hi_speed ----------------------------------*/

void spi_hi_speed (BOOL on) {
  /* Set a SPI clock to low/high speed for SD/MMC. */
  U32 clk;

  if (on == __TRUE) {
    /* Max. 20 MBit used for Data Transfer. */
    clk = 12500000;                     /* Maximum allowed clock is 12.5MHz. */
  }
  else {
    /* Max. 400 kBit used in Card Initialization. */
    clk = 400000;
  }
  SSIDisable(SSIx_BASE);
  SSIConfig (SSIx_BASE, SSI_FRF_MOTO_MODE_0, SSI_MODE_MASTER, clk, 8);
  SSIEnable (SSIx_BASE);
}


/*--------------------------- spi_ss ----------------------------------------*/

void spi_ss (U32 ss) {
  /* Enable/Disable SPI Chip Select (drive it high or low). */

  HWREG(CS_PORT + (GPIO_O_DATA + (SSI_CS << 2))) = ss ? SSI_CS : 0;
}


/*--------------------------- spi_send --------------------------------------*/

U8 spi_send (U8 outb) {
  /* Write and Read a byte on SPI interface. */

  while(!(HWREG(SSIx_BASE + SSI_O_SR) & SSI_SR_TNF));
  HWREG(SSI0_BASE + SSI_O_DR) = outb;

  while(!(HWREG(SSIx_BASE + SSI_O_SR) & SSI_SR_RNE));
  return (HWREG(SSIx_BASE + SSI_O_DR));
}


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
