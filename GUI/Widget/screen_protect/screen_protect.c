#include "includes.h"

static void disp_time (void)
{
	TIME time;	

	pcf8563_read (&time);

    font_printf (8, 1, 0, "%d年%02d月%02d日", time.year, time.month, time.day);
    font_printf (32, 3, 0, "%02d:%02d:%02d", time.hour, time.minute, time.second);

	switch (time.week)
	{
		case 1:
			font_print (40, 5, "星期一", 0);
			break;
		case 2:
			font_print (40, 5, "星期二", 0);
			break;
		case 3:
			font_print (40, 5, "星期三", 0);
			break;
		case 4:
			font_print (40, 5, "星期四", 0);
			break;
		case 5:
			font_print (40, 5, "星期五", 0);
			break;
		case 6:
			font_print (40, 5, "星期六", 0);
			break;
		case 0:
			font_print (40, 5, "星期日", 0);
			break;								
		default:
			break;
	}
}

void screen_protect_main (void* pdata)
{
    INT8U cnt = 0;
	INT8U cnt2 = 0;

    //初始化
	lcd_clear ();
	disp_time ();
    
    //消息循环
    while (1)
    {
        //刷新
        if (cnt == 4)
        {
             cnt = 0;
             disp_time ();   
        }
		else
		{
		 	cnt++;
		}

		if (cnt2 == 15)
        {
             cnt2 = 0;
             lcd_light (0);    
        }
		else
		{
		 	cnt2++;
		}
        
        switch (key_msg_get ())
        {
			//长按显示时间
			case KEY_UP:     
	        case KEY_DOWN:
			case KEY_LEFT:
			case KEY_RIGHT:    
	        case KEY_ENTER:  
	        case KEY_RETURN:
				lcd_light (1); 
				OSTimeDly (50);
				widget_run ("desktop", 0);
				break;
	
			default:
	            break;   
        }
    }     
}    
