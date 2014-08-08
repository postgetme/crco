#include "includes.h"
#include "list_design.h" 

/*
typedef struct
{
    INT8U x;            //x坐标		
    INT8U y;            //y坐标		y=255,右对齐   y=254,居中
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

static INT8U list_cnt = 0;
static INT8U list_disp_cnt = 0;
static INT8U list_current_sel = 0;
static EDIT_INFO edit_info; 

INT8U find (INT8U index)
{
    INT8U i;
    INT8U cnt = 0;
    
    for (i = 0; i < list_cnt; i++)
    {
        if (list_info[i].disp_flag == LIST_DISP_ON)
        {
            cnt++;
        }
        
        if ((cnt - 1) == index)
        {
            return i;   
        }        
    }
    
    return 0;
}

void disp_list (INT8U sel)
{
    INT8U line1_index;
    INT8U line2_index;
  
    INT8U flag = 0;
	INT8U now_sel = 0;

	EDIT_RET* p_edit_ret;    
    lcd_clear ();

	bar_init ();

	list_current_sel = sel;

    bar_width_set (list_disp_cnt);
	bar_pos (sel);
    
    if (sel % 2 == 0)
    {
        now_sel = 1;
		
		line1_index = find (sel);
        
        if (sel + 1 == list_disp_cnt)
        {
            flag = 1;
        }
        else
        {
            line2_index = find (sel + 1);
        }
    }
    else
    {
        now_sel = 2;
		
		line1_index = find (sel - 1);
        line2_index = find (sel); 
    }

    //显示第一项名称
    if (widget_lang_get () == WIDGET_CHINESE)
	{
		font_printf (0, 0, list_info[line1_index].var_attr == LIST_RO && now_sel == 1 ? 1:0, "%s", list_info[line1_index].ch_name);
	}
	else
	{
		font_printf (0, 0, list_info[line1_index].var_attr == LIST_RO && now_sel == 1 ? 1:0, "%s", list_info[line1_index].en_name);
    }
	//显示第一项值
    edit_info.x = 255;
	edit_info.y	= 2;
	if (widget_lang_get () == WIDGET_CHINESE)
	{
		edit_info.expr = list_info[line1_index].ch_range_expr;
		edit_info.format_expr = list_info[line1_index].ch_format_expr;
	}
	else
	{
		edit_info.expr = list_info[line1_index].en_range_expr;
		edit_info.format_expr = list_info[line1_index].en_format_expr;
	}
	edit_info.step_val = list_info[line1_index].step_val;
	edit_info.start_val	= *list_info[line1_index].var_p;
	edit_info.style = 3;
	p_edit_ret = (EDIT_RET*)widget_enter_sub ("edit", &edit_info);

    if (flag == 0)
    {
        //显示第二项名称
		if (widget_lang_get () == WIDGET_CHINESE)
        {
			font_printf (0, 4, list_info[line2_index].var_attr == LIST_RO && now_sel == 2 ? 1:0, "%s", list_info[line2_index].ch_name);
		}
		else
		{
			font_printf (0, 4, list_info[line2_index].var_attr == LIST_RO && now_sel == 2 ? 1:0, "%s", list_info[line2_index].en_name);
		}
        //显示第二项值
		edit_info.x = 255;
		edit_info.y	= 6;
		if (widget_lang_get () == WIDGET_CHINESE)
		{
			edit_info.expr = list_info[line2_index].ch_range_expr;
			edit_info.format_expr = list_info[line2_index].ch_format_expr;
		}
		else
		{
			edit_info.expr = list_info[line2_index].en_range_expr;
			edit_info.format_expr = list_info[line2_index].en_format_expr;
		}
		edit_info.step_val = list_info[line2_index].step_val;
		edit_info.start_val	= *list_info[line2_index].var_p;
		edit_info.style = 3;
		p_edit_ret = (EDIT_RET*)widget_enter_sub ("edit", &edit_info);
	}

	//进入edit控件
	if (list_info[line1_index].var_attr == LIST_RW && now_sel == 1)
	{
		edit_info.x = 255;
		edit_info.y	= 2;
		if (widget_lang_get () == WIDGET_CHINESE)
		{
			edit_info.expr = list_info[line1_index].ch_range_expr;
			edit_info.format_expr = list_info[line1_index].ch_format_expr;
		}
		else
		{
			edit_info.expr = list_info[line1_index].en_range_expr;
			edit_info.format_expr = list_info[line1_index].en_format_expr;
		}
		edit_info.step_val = list_info[line1_index].step_val;

		re_in1:
		edit_info.start_val	= *list_info[line1_index].var_p;
		edit_info.style = 2;
		
		p_edit_ret = (EDIT_RET*)widget_enter_sub ("edit", &edit_info);

		switch (p_edit_ret->msg)
        {
			case KEY_ENTER:
				*list_info[line1_index].var_p = p_edit_ret->ret;
				list_on_change (list_info[line1_index].id);
				OSTimeDly (20);							   				
				//goto re_in1;
				disp_list (list_current_sel);

			default:
	            break; 
        }
		
		key_msg_flush ();
		key_msg_post (p_edit_ret->msg);		

	}
	else if (flag == 0 && list_info[line2_index].var_attr == LIST_RW && now_sel == 2)
	{
		edit_info.x = 255;
		edit_info.y	= 6;
		if (widget_lang_get () == WIDGET_CHINESE)
		{
			edit_info.expr = list_info[line2_index].ch_range_expr;
			edit_info.format_expr = list_info[line2_index].ch_format_expr;
		}
		else
		{
			edit_info.expr = list_info[line2_index].en_range_expr;
			edit_info.format_expr = list_info[line2_index].en_format_expr;
		}
		edit_info.step_val = list_info[line2_index].step_val;
		
		re_in2:
		edit_info.start_val	= *list_info[line2_index].var_p;
		edit_info.style = 2;

		p_edit_ret = (EDIT_RET*)widget_enter_sub ("edit", &edit_info);

		switch (p_edit_ret->msg)
        {
			case KEY_ENTER:
				*list_info[line2_index].var_p = p_edit_ret->ret;
				list_on_change (list_info[line2_index].id);
				OSTimeDly (20);
				//goto re_in2;
				disp_list (list_current_sel);

			default:
	            break; 
        }

		key_msg_flush ();
		key_msg_post (p_edit_ret->msg);	
	}
	else
	{
		;
	}
}

void list_up (void)
{
	if (list_current_sel == 0)
	{
		list_current_sel = list_disp_cnt - 1;
	}
	else
	{
		list_current_sel--;	
	}
	
	disp_list (list_current_sel);    
}

void list_down (void)
{
	if (list_current_sel == list_disp_cnt - 1)
	{
		list_current_sel = 0;
	}
	else
	{
		list_current_sel++;
	}
	
	disp_list (list_current_sel);    
}

void list_main (void* pdata)
{
    INT8U i;

	list_cnt = sizeof (list_info) / sizeof (LIST_INFO);
    
    list_disp_cnt = list_cnt;
    
    if ((INT32U)pdata == 1)         //只显示只读项
    {
        for (i = 0; i < list_cnt; i++)
        {
            if (list_info[i].var_attr == LIST_RW)
            {
                list_info[i].disp_flag = LIST_DISP_OFF;
                list_disp_cnt--;
            }   
        }  
    }
    else if ((INT32U)pdata == 2)    //只显示读写项
    {
        for (i = 0; i < list_cnt; i++)
        {
            if (list_info[i].var_attr == LIST_RO)
            {
                list_info[i].disp_flag = LIST_DISP_OFF;
                list_disp_cnt--;
            }   
        }         
    }
	else //全部显示
	{
	 	;
	}

	disp_list (0);
    
    while (1)
    {
		switch (key_msg_get ())
        {
	        case KEY_UP: 
	            list_up ();
	            break;
	                
	        case KEY_DOWN:
	            list_down ();
	            break;
	            
			case KEY_LEFT:
			case KEY_RIGHT:   
	        case KEY_ENTER: 
				break;
			 
	        case KEY_RETURN:
				widget_run ("desktop", (void*)0);
				return;
	
			default:
	            break; 
        }
    }
}
