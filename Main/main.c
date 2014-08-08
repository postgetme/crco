#include "includes.h"

static OS_STK main_task_stk[MAIN_TASK_STK_SIZE];

static void main_task (void* pdat)
{
	app_init ();	

	OSTaskDel (MAIN_TASK_PRIO);
}

int main(void)
{				
	sys_init ();
		
	OSInit();

	OSTaskCreate(main_task, (void*)0, &main_task_stk[MAIN_TASK_STK_SIZE - 1], MAIN_TASK_PRIO);

	OSStart(); 	  

	while(1); 
}
