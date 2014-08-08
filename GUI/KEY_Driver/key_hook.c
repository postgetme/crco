//KEY_2.0 Designed by Mei_Lin at 20101224

#include "includes.h"

//以下是hook用于简单背光控制，可根据实际应用作修改

static OS_EVENT* os_sem_key_hook; 
static OS_STK KEY_HOOK_TASK_STK[KEY_HOOK_TASK_STK_SIZE];

static void KeyHookTask (void* pdata)
{
	INT8U err;
	
	while (1)
	{
		OSSemPend (os_sem_key_hook, 30000, &err);
		if (err == OS_TIMEOUT)
		{
			widget_run ("screen_protect", 0);
		}
		else
		{
			lcd_light (1);
		}
	
	}
}

void key_hook_init (void)
{
	os_sem_key_hook = OSSemCreate (0);
	OSTaskCreate(KeyHookTask, (void*)0, &KEY_HOOK_TASK_STK[KEY_HOOK_TASK_STK_SIZE - 1], KEY_HOOK_TASK_PRIO);	
}

void key_hook (INT32U msg)
{
	OSSemPost (os_sem_key_hook);	
} 
