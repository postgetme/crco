#include "includes.h"

static OS_STK desktop_task1_stk[DESKTOP_TASK1_STK_SIZE];
static OS_STK desktop_task2_stk[DESKTOP_TASK2_STK_SIZE];

static void desktop_task1 (void* pdata)
{
    while (1)
    {
        font_print (8, 3, "desktop task 1", 0);
        OSTimeDly (50);  
        font_print (8, 3, "desktop task 1", 1);
        OSTimeDly (50);  
    }
}

static void desktop_task2 (void* pdata)
{
    while (1)
    {
        font_print (8, 6, "desktop task 2", 0);
        OSTimeDly (25);  
        font_print (8, 6, "desktop task 2", 1);
        OSTimeDly (25);  
    }
}

void desktop_main (void* pdata)
{
    //≥ı ºªØ
	lcd_clear ();
    font_print (32, 0, "DESKTOP", 0);
        
    OSTaskCreate (desktop_task1, 0, &desktop_task1_stk[DESKTOP_TASK1_STK_SIZE - 1], DESKTOP_TASK1_PRIO);
    OSTaskCreate (desktop_task2, 0, &desktop_task2_stk[DESKTOP_TASK2_STK_SIZE - 1], DESKTOP_TASK2_PRIO);
    
    while (1)
    {
		switch (key_msg_get ())
        {
	        case KEY_UP:     
	        case KEY_DOWN:
			case KEY_LEFT:
			case KEY_RIGHT:   
	        case KEY_ENTER:  
	        case KEY_RETURN:
				widget_run ("menu", 0);
				break;
	
			default:
	            break; 
        }
    }    
}

void desktop_destroy (void)
{
    OSTaskDel (DESKTOP_TASK1_PRIO);
    OSTaskDel (DESKTOP_TASK2_PRIO);    
}
