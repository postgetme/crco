#include "includes.h"

OS_STK app_task0_stk[APP_TASK0_STK_SIZE];
OS_STK app_task1_stk[APP_TASK1_STK_SIZE];
OS_STK app_task2_stk[APP_TASK2_STK_SIZE];
OS_STK app_task3_stk[APP_TASK3_STK_SIZE];
OS_STK app_task4_stk[APP_TASK4_STK_SIZE];
OS_STK app_task5_stk[APP_TASK5_STK_SIZE];

extern void app_task1(void* pdat);
extern void app_task2(void* pdat);
extern void app_task3(void* pdat);
extern void app_task4(void* pdat);
extern void app_task5(void* pdat);

static INT8S fs_init (void)
{
   INT32U retv;

   retv = finit ();

   spi_hi_speed (__TRUE); 

   if (retv == 0) {
      retv = fcheck ("S:");
   }
   else
   {
      retv = fformat ("S:");
      if (retv == 0)
      {
         return 0;
      }
      else
      {
         return -1;
      }
   }

   return 0;
}

static bsp_init (void)
{
	rg_led_init ();

	g_led (0);

	#if FS_EN == 1
		fs_init ();
	#endif

	#if SHELL_EN == 1
 		shell_init ();
	#endif

	

	#if TCP_EN == 1
		init_TcpNet ();
	#endif
				
	#if KEY_EN == 1
 		key_init ();
	#endif
	
	#if RESET_KEY_EN == 1
		reset_key_init ();
	#endif

	#if GUI_EN == 1
		lcd_init (); 
		lcd_write_led (0);	  
		widget_init ();
	#endif

	I2CInit(0);

	#if RTC_EN == 1
		pcf8563_init ();
	#endif
}

static void app_task0(void* pdat)
{
	while (1)
	{
	#if TCP_EN == 1
		main_TcpNet();
	#else
		OSTimeDly (100000);
	#endif
	}
}

void app_init (void)
{
	bsp_init ();
	
	OSTaskCreate(app_task0, (void*)0, &app_task0_stk[APP_TASK0_STK_SIZE - 1], APP_TASK0_PRIO);

//	OSTaskCreate(app_task1, (void*)0, &app_task1_stk[APP_TASK1_STK_SIZE - 1], APP_TASK1_PRIO);	
//	OSTaskCreate(app_task2, (void*)0, &app_task2_stk[APP_TASK2_STK_SIZE - 1], APP_TASK2_PRIO);
//	OSTaskCreate(app_task3, (void*)0, &app_task3_stk[APP_TASK3_STK_SIZE - 1], APP_TASK3_PRIO);
//	OSTaskCreate(app_task4, (void*)0, &app_task4_stk[APP_TASK4_STK_SIZE - 1], APP_TASK4_PRIO);	
//	OSTaskCreate(app_task5, (void*)0, &app_task5_stk[APP_TASK5_STK_SIZE - 1], APP_TASK5_PRIO);
}
