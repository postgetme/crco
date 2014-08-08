#include	"uCOS-II/Source/ucos_ii.h"
#include "includes.h"
/*
*********************************************************************************************************
*                                   APPLICATION-DEFINED HOOKS
*
* Description: These functions are called by the hooks in os_cpu_c.c.
*
* Returns    : none
*********************************************************************************************************
*/ 
#if (OS_APP_HOOKS_EN > 0)
void  App_TaskCreateHook (OS_TCB *ptcb)
{

}

void  App_TaskDelHook (OS_TCB *ptcb)
{
    (void)ptcb;
}

#if OS_VERSION >= 251
void  App_TaskIdleHook (void)
{

}
#endif


void  App_TaskStatHook (void)
{

}

#if OS_TASK_SW_HOOK_EN > 0
void  App_TaskSwHook (void)
{

}
#endif

#if OS_VERSION >= 204
void  App_TCBInitHook (OS_TCB *ptcb)
{
    (void)ptcb;
}
#endif


#if OS_TIME_TICK_HOOK_EN > 0

void  App_TimeTickHook (void)
{
#if TCP_EN == 1
	static INT8U count = 0;

	if (count > 9)
	{
		timer_tick();
		count = 0;
	}
	else
	{
		count++;
	}
#endif
}

#endif
#endif
