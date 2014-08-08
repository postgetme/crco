#include "includes.h"

#define RED_LED     "pd5"
#define GREEN_LED   "pd6"

static OS_EVENT* os_sem_rg_led;
static OS_STK RG_LED_TASK1_STK[RG_LED_TASK1_STK_SIZE];

static void rg_led_task1 (void* pdata)
{
	INT8U err;
	
	while (1)
	{
        OSSemPend (os_sem_rg_led, 50, &err);

		if (err == OS_TIMEOUT)
		{
			io_reverse (GREEN_LED, 0);    
		}
		else
		{
			io_write (GREEN_LED, 1);
			io_write (RED_LED, 0);
			OSTimeDly (5);
			io_write (RED_LED, 1);
			OSTimeDly (5);
			while (OSSemAccept(os_sem_rg_led));    
		}
	}
}

void rg_led_init (void)
{
	io_init (RED_LED, "out");
	io_init (GREEN_LED, "out");
	
	os_sem_rg_led = OSSemCreate (0);
	OSTaskCreate(rg_led_task1, (void*)0, &RG_LED_TASK1_STK[RG_LED_TASK1_STK_SIZE - 1], RG_LED_TASK1_PRIO);
}

void r_led_flash (void)
{
    OSSemPost (os_sem_rg_led);
}

void r_led (INT8U onoff)
{
	io_write (RED_LED, onoff);
}

void g_led (INT8U onoff)
{
	io_write (GREEN_LED, onoff);
}


