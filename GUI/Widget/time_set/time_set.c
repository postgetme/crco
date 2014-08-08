#include "includes.h"

/*
typedef struct
{
    INT8U x;            //x坐标
    INT8U y;            //y坐标
    char* expr;         //表达示，"[0-5000]"/"(是|否)"
    char* format_expr;  //格式表达式，"%04d毫安"/"%s"
    FP32 step_val;      //步进值
    FP32 start_val;     //起始值
    INT8U style;        //风格，风格1，上下键调值，风格2，左右键调值    
}EDIT_INFO;

typedef struct
{
    INT32U msg;
    FP32 ret;   
}EDIT_RET;
*/

EDIT_INFO edit_info[7] = 
{
    {8, 1, "[2000-2100]", "%04.0f", 1, 0, 2}, //年
    {56, 1, "[1-12]", "%02.0f", 1, 0, 2},       //月
    {88, 1, "[1-31]", "%02.0f", 1, 0, 2},       //日
    {32, 3, "[0-23]", "%02.0f", 1, 0, 2},        //时
    {56, 3, "[0-59]", "%02.0f", 1, 0, 2},        //分
    {80, 3, "(00|15|30|45)", "%s", 1, 0, 2},         //秒
    {40, 5, "(星期日|星期一|星期二|星期三|星期四|星期五|星期六)", "%s", 1, 0, 2},   //星期
};

EDIT_RET* p_edit_ret;

void time_set_main (void* pdata)
{
    INT8U index = 0;
    TIME time;	

    //初始化
    lcd_clear ();  
      
	pcf8563_read (&time);	
	font_printf (8, 1, 0, "%d年%02d月%02d日", time.year, time.month, time.day);
    font_printf (32, 3, 0, "%02d:%02d:%02d", time.hour, time.minute, 0);

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
	
	edit_info[0].start_val = time.year;
    edit_info[1].start_val = time.month;
    edit_info[2].start_val = time.day;
    edit_info[3].start_val = time.hour;
    edit_info[4].start_val = time.minute;
    edit_info[5].start_val = 0;
    edit_info[6].start_val = time.week;

    //消息循环
    while (1)
    {
		p_edit_ret = (EDIT_RET*)widget_enter_sub ("edit", &edit_info[index]);
		
		switch (index)
		{
		    case 0:
		        time.year = (INT16U)p_edit_ret->ret; 
		        edit_info[0].start_val = time.year;
		        break;
		        
		    case 1:
		        time.month = (INT8U)p_edit_ret->ret; 
		        edit_info[1].start_val = time.month;
		        break;
		        
		    case 2:
		        time.day = (INT8U)p_edit_ret->ret; 
		        edit_info[2].start_val = time.day;
		        break;
		        
		    case 3:
		        time.hour = (INT8U)p_edit_ret->ret; 
		        edit_info[3].start_val = time.hour;
		        break;
		        
		    case 4:
		        time.minute = (INT8U)p_edit_ret->ret; 
		        edit_info[4].start_val = time.minute;
		        break;	
		        		        			        
		    case 5:
		        time.second = 15 * (INT8U)p_edit_ret->ret; 
		        edit_info[5].start_val = time.second;
		        break;	
		        
		    case 6:
		        time.week = (INT8U)p_edit_ret->ret; 
		        edit_info[6].start_val = time.week;
		        break;
				
			default:
				break;			        		        			        
		}
		
		switch (p_edit_ret->msg)
        {
			case KEY_UP:
				index--;
			    if (index == 255)
			    {
			        index = 6;   
			    }
			    break;
			    
			case KEY_DOWN:
				index++;
			    if (index == 7)
			    {
			        index = 0;   
			    }
			    break;  
			     
	        case KEY_ENTER:
	            pcf8563_write (&time);
               	//break;

            case KEY_RETURN:
				widget_exit_sub (0);
				return;
	
			default:
	            break; 
        }
    } 
}
