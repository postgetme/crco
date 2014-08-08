#include "includes.h"

static void disp_time (void)
{
	TIME time;	

	pcf8563_read (&time);

    font_printf (8, 1, 0, "%d��%02d��%02d��", time.year, time.month, time.day);
    font_printf (32, 3, 0, "%02d:%02d:%02d", time.hour, time.minute, time.second);

	switch (time.week)
	{
		case 1:
			font_print (40, 5, "����һ", 0);
			break;
		case 2:
			font_print (40, 5, "���ڶ�", 0);
			break;
		case 3:
			font_print (40, 5, "������", 0);
			break;
		case 4:
			font_print (40, 5, "������", 0);
			break;
		case 5:
			font_print (40, 5, "������", 0);
			break;
		case 6:
			font_print (40, 5, "������", 0);
			break;
		case 0:
			font_print (40, 5, "������", 0);
			break;								
		default:
			break;
	}
}

void screen_protect_main (void* pdata)
{
    INT8U cnt = 0;
	INT8U cnt2 = 0;

    //��ʼ��
	lcd_clear ();
	disp_time ();
    
    //��Ϣѭ��
    while (1)
    {
        //ˢ��
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
			//������ʾʱ��
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
