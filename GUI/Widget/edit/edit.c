#include "includes.h"

static EDIT_INFO* p_edit_info;
static EDIT_RET edit_ret;

static FP32 max;
static FP32 min;
static FP32 step;
static FP32 current_val;
static char* format_expr;
static INT8U x;
static INT8U y;
static INT8U style;

#define TYPE_FLOAT 1
#define TYPE_ENUM  2
static INT8U expr_type;//1,FP32 2,enum

static char* str;
static INT8U len;

static char s[16];

static INT8U clear_x;
static INT8U clear_y;
static INT8U clear_len;

static INT8U get_enum_item_sum (void)
{
    INT8U i;
    INT8U cnt = 0;
    
    for (i = 1; i < len - 1; i++)
    {
        if (str[i] == '|')
        {
            cnt++;  
        }    
    }
    
    return cnt + 1;    
}

static INT8S get_enum_separator_position (INT8U index)//从1开始
{
    INT8U i;
    INT8U cnt = 0;
    
    if (index < get_enum_item_sum ())
    {
        for (i = 1; i < len - 1; i++)
        {
            if (str[i] == '|')
            {
                cnt++;
                
                if (cnt == index)
                {
                    return i;
                }     
            }        
        }            
    }
    else if (index == get_enum_item_sum ())
    {
        return len - 1;    
    }

    return -1;   
}

static char* get_enum_item_str (INT8U index)//从0开始
{
    static char enum_str[32];
    
    if (index >= get_enum_item_sum ())
    {
        return 0;      
    }
    
    memset (enum_str, 0, sizeof (enum_str));
    
    if (index == 0)
    {
        memcpy (enum_str, str + 1, get_enum_separator_position (1) - 1);    
    }
    else
    {
        memcpy (enum_str, str + get_enum_separator_position (index) + 1, get_enum_separator_position (index + 1) - get_enum_separator_position (index) - 1);     
    }    
    
    return enum_str;
}

static INT8S max_min_init (void)
{
    INT8U i;
    char max_str[10];
    char min_str[10];
    
    if (expr_type == TYPE_FLOAT) 
    {
        for (i = 1; i < len - 1; i++)
        {
            if (str[i] == '-')
            {
                memset (max_str, 0, 10);
                memset (min_str, 0, 10);
                
                memcpy (min_str, str + 1, i - 1);
                memcpy (max_str, str + i + 1, len - i);
                
                min = atof (min_str);
                max = atof (max_str);
                
                return 0;   
            }    
        }
    }
    else if (expr_type == TYPE_ENUM)
    {
        min = 0;
        max = get_enum_item_sum () - 1;  
    }      

    return -1;
}

static INT8S init (void)
{
    str = p_edit_info->expr;
    len = strlen (str);
    
    step = p_edit_info->step_val;
    current_val = p_edit_info->start_val;
    x = p_edit_info->x;
    y = p_edit_info->y;
    format_expr = p_edit_info->format_expr;
    style = p_edit_info->style; 
    
    if (str[0] == '[' && str[len - 1] == ']')
    {
        expr_type  = TYPE_FLOAT;
    }
    else if (str[0] == '(' && str[len - 1] == ')')
    {
        expr_type = TYPE_ENUM;
        
        step = 1;
    }
    else
    {
        return -1;   
    }
    
    if (max_min_init () == 0)
    {
        return 0;    
    }   
    else
    {
        return -1;   
    }            
}

static void draw (INT8U mode)
{
	INT8U len;
	
	if (expr_type == TYPE_FLOAT) 
    {
        len = snprintf (s, 16, format_expr, current_val);

		if (x == 255)	//右对齐
		{
			font_printf (120 - len * 8, y, mode, format_expr, current_val);

			clear_x = 120 - len * 8;
		}
		else
		{
			font_printf (x, y, mode, format_expr, current_val); 

			clear_x = x;  
		}

		clear_y = y;
		clear_len = len;
    }
    else if (expr_type == TYPE_ENUM)
    {
        len = snprintf (s, 16, "%s", get_enum_item_str (current_val));

		if (x == 255)	//右对齐
		{
			font_printf (120 - len * 8, y, mode, "%s", get_enum_item_str (current_val));

			clear_x = 120 - len * 8;   
		}
		else
		{
        	font_printf (x, y, mode, /*format_expr*/"%s", get_enum_item_str (current_val)); 
			
			clear_x = x;    
		}

		clear_y = y;
		clear_len = len;
    }   
}

static void clear (void)
{
 	INT8U i;
	
	for (i = 0; i < clear_len; i++)
	{
	 	font_printf (clear_x + i * 8, clear_y, 0, " "); 
	}
}

static void increase (void)
{
    current_val += step;
    
    if (current_val > max)
    {
        current_val = min;
    } 

	clear ();
    draw (1);        
}

static void decrease (void)
{
    current_val -= step;
    
    if (current_val < min)
    {
        current_val = max;
    }

	clear ();
    draw (1);     
}

void edit_main (void* pdata)
{
    INT32U msg;
    
    //初始化
    p_edit_info = (EDIT_INFO*)pdata;
    init ();
    
    //消息循环
    if (style == 1)
    {
        goto style1;   
    }
    else if (style == 2)
    {
        goto style2;
    }
    else
    {
        goto style3;  
    }
    
style1:    
    while (1)
    {
	    draw (1); 
		
		switch (msg = key_msg_get ())
        {
	        case KEY_UP:
	            increase (); 
	            break;  
	              
	        case KEY_DOWN:
	            decrease ();
	            break;
	            
	        case KEY_LEFT: 
	        case KEY_RIGHT:
	        case KEY_ENTER:
	        case KEY_RETURN:
                edit_ret.msg = msg;
                edit_ret.ret = current_val;
				draw (0);
                widget_exit_sub (&edit_ret);	            
				return;
	
			default:
	            break; 
        }
    } 
    
style2:    
    while (1)
    {
	    draw (1); 
		
		switch (msg = key_msg_get ())
        {
	        case KEY_LEFT: 
	            decrease ();
	            break;  
	              
	        case KEY_RIGHT:
	            increase (); 
	            break;	
	            
	        case KEY_UP: 
	        case KEY_DOWN:
	        case KEY_ENTER:
	        case KEY_RETURN:
                edit_ret.msg = msg;
                edit_ret.ret = current_val;
				draw (0);
				widget_exit_sub (&edit_ret);
				return;

			default:
	            break; 
        }
    } 
    
style3:    
	draw (0);
	widget_exit_sub (0);
}
