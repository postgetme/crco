#include "includes.h"

static OS_EVENT* os_sem_reset_key; 
static OS_STK RESET_KEY_TASK1_STK[RESET_KEY_TASK1_STK_SIZE];
static OS_STK RESET_KEY_TASK2_STK[RESET_KEY_TASK2_STK_SIZE];

static INT8U count = 0;

static void reset_key_task1 (void* pdata)
{
	while (1)
	{
        switch (key_msg_get ())
        {
            case KEY_1_DOWN:
				OSSemPost (os_sem_reset_key);

				count++;            
	            if (count == 5)
	            {                    
	                sys_para_reset ();

					//复位前关闭PLL，否则可能死机
					SysCtlClockSet(SYSCTL_SYSDIV_4| SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ);
					SysCtlClockSet(SYSCTL_SYSDIV_4| SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ);
					SysCtlClockSet(SYSCTL_SYSDIV_4| SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ);
					SysCtlClockSet(SYSCTL_SYSDIV_4| SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ);
					SysCtlClockSet(SYSCTL_SYSDIV_4| SYSCTL_USE_OSC | SYSCTL_OSC_MAIN | SYSCTL_XTAL_6MHZ);
	                SysCtlReset();
	            }

                break;
                
            default:
                break;   
        }	
	}
}

static void reset_key_task2 (void* pdata)
{
	INT8U err;
	
	while (1)
	{
        OSSemPend (os_sem_reset_key, 50, &err);
        if (err == OS_TIMEOUT)
        {
            count = 0;       
        }
	}
}

void reset_key_init(void)
{
	os_sem_reset_key = OSSemCreate (0);
	OSTaskCreate(reset_key_task1, (void*)0, &RESET_KEY_TASK1_STK[RESET_KEY_TASK1_STK_SIZE - 1], RESET_KEY_TASK1_PRIO);
	OSTaskCreate(reset_key_task2, (void*)0, &RESET_KEY_TASK2_STK[RESET_KEY_TASK2_STK_SIZE - 1], RESET_KEY_TASK2_PRIO);
}
