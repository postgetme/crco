/*----------------------------------------------------------------------------
 *      R T L   F l a s h   F i l e   S y s t e m   E x a m p l e
 *----------------------------------------------------------------------------
 *      Name:    SERIAL.C
 *      Purpose: Serial Input Output for Luminary LM3S6965
 *      Rev.:    V3.20
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2008 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include "includes.h"

#if SHELL_UART_PORT == 0		
    #define UART_BASE UART0_BASE
#elif SHELL_UART_PORT == 1		
    #define UART_BASE UART1_BASE
#elif SHELL_UART_PORT == 2		
    #define UART_BASE UART2_BASE
#else
    #define UART_BASE UART0_BASE
#endif

/*----------------------------------------------------------------------------
 *       init_serial:  Initialize Serial Interface
 *---------------------------------------------------------------------------*/
void init_serial (void) {
    /* Initialize the serial interface */
#if SHELL_UART_PORT == 0		
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinTypeUART(GPIO_PORTA_BASE , GPIO_PIN_0 | GPIO_PIN_1);

#elif SHELL_UART_PORT == 1		
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
    GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_2|GPIO_PIN_3);

#elif SHELL_UART_PORT == 2		
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
    GPIOPinTypeUART(GPIO_PORTG_BASE, GPIO_PIN_0|GPIO_PIN_1);
    
#else	
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    GPIOPinTypeUART(GPIO_PORTA_BASE , GPIO_PIN_0 | GPIO_PIN_1);    
#endif
    
    UARTConfigSet(UART_BASE, SHELL_UART_BPS, (UART_CONFIG_WLEN_8 |
                                                 UART_CONFIG_STOP_ONE |
                                                 UART_CONFIG_PAR_NONE));
                                                 
    io_init (SHELL_UART_485_CTL_IO, "out");
}

/*----------------------------------------------------------------------------
 *       sendchar:  Write a character to Serial Port
 *---------------------------------------------------------------------------*/
int sendchar (int ch) {
    io_write (SHELL_UART_485_CTL_IO, 1);
    
    if (ch == '\n')
    {
        UARTCharPut (UART_BASE, '\r');        /* output CR                    */
    }
    UARTCharPut (UART_BASE, ch);
    
    io_write (SHELL_UART_485_CTL_IO, 0);
    
    return(ch);
}

/*----------------------------------------------------------------------------
 *       getkey:  Read a character from Serial Port
 *---------------------------------------------------------------------------*/
int getkey (void)
{
    return(UARTCharGet (UART_BASE));
}

void serial_disable (void)
{
    UARTDisable(UART_BASE);
}

int unpend_getkey (void)
{
    return(UARTCharGetNonBlocking (UART_BASE));
}

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
