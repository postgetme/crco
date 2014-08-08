#include "includes.h"

static void jtag_lock (void)
{
	#define   GPIOB_BASE      0x40005000
	#define   GPIOC_BASE      0x40006000
	#define   GPIOBLOCK   (*((volatile unsigned long *)(GPIOB_BASE+0x520)))
	#define   GPIOBCR     (*((volatile unsigned long *)(GPIOB_BASE+0x524)))
	#define   GPIOBAFSEL  (*((volatile unsigned long *)(GPIOB_BASE+0x420))) 
	#define   GPIOCLOCK   (*((volatile unsigned long *)(GPIOC_BASE+0x520)))
	#define   GPIOCCR     (*((volatile unsigned long *)(GPIOC_BASE+0x524))) 
	#define   GPIOCAFSEL  (*((volatile unsigned long *)(GPIOC_BASE+0x420)))

    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOC);

    GPIOBLOCK = 0x1ACCE551;
    GPIOBCR   = 0xFF;
    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_7, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_7, GPIO_DIR_MODE_IN);
    GPIOBLOCK  = 0x00;   
    GPIOCLOCK = 0x1ACCE551;
    GPIOCCR   = 0xFF;
    GPIOPadConfigSet(GPIO_PORTC_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIODirModeSet(GPIO_PORTC_BASE, GPIO_PIN_0|GPIO_PIN_1, GPIO_DIR_MODE_OUT);
    GPIOCLOCK = 0x00;   

    GPIOPadConfigSet(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
    GPIODirModeSet(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_DIR_MODE_IN);    
    GPIOIntTypeSet(GPIO_PORTB_BASE, GPIO_PIN_5, GPIO_FALLING_EDGE);
    GPIOPinIntEnable(GPIO_PORTB_BASE, GPIO_PIN_5); 

    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_0, ~GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_1, GPIO_PIN_1);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_0, GPIO_PIN_0);
    GPIOPinWrite(GPIO_PORTC_BASE, GPIO_PIN_1, ~GPIO_PIN_1);
}

static void  TmrTickInit (void)
{
    CPU_INT32U  cnts;	

    cnts = (CPU_INT32U)SysCtlClockGet() / OS_TICKS_PER_SEC;
    SysTickPeriodSet(cnts);
    SysTickEnable();
    SysTickIntEnable();
}

void sys_init(void)
{
	SysCtlLDOSet(SYSCTL_LDO_2_75V);
	SysCtlClockSet(SYSCTL_SYSDIV_4| SYSCTL_USE_PLL | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ);
	IntSysBootRegister(0x0000C000L);	 
	TmrTickInit();
}

void  ucos_tick_isr (void)
{
    OS_CPU_SR  cpu_sr;

    OS_ENTER_CRITICAL();                         /* Tell uC/OS-II that we are starting an ISR          */
    OSIntNesting++;
    OS_EXIT_CRITICAL();

    OSTimeTick();                                /* Call uC/OS-II's OSTimeTick()                       */

    OSIntExit();                                 /* Tell uC/OS-II that we are leaving the ISR          */
} 

