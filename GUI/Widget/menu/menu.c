#include "includes.h"
#include "menu_design.h"

static INT8U menu_cnt;
static INT8U roll_order[MENU_MAX_CNT_IN_LAYER];
static INT8U roll_cnt = 0;
static INT8U roll_pos = 0;

static INT8U* record_node[MENU_MAX_LAYER];
static INT8U record_pos[MENU_MAX_LAYER];
static INT8U record_layer = 0;
static INT8U* current_node = 0;
static INT8U back_flag = 0;

static INT8U get_dot_cnt (INT8U* node)
{
    INT8U i;
    INT8U len;
    INT8U cnt = 0;
    
    len = strlen (node);
        
    for (i = 0; i < len; i++)
    {
        if (node[i] == '.')
        {
            cnt++;   
        }
    }
    
    return cnt;
}

static INT8S is_match (INT8U* node1, INT8U* node2)
{
    INT8U i;
	INT8U len;
    INT8U cnt = 0;
    
    len = strlen (node1);
    
    for (i = 0; i < len; i++)
    {
        if (node1[i] == node2[i])
        {
            cnt++;
        }
    }
    
    if (cnt == len && node2[len] == '.')
    {
        return 0;   
    }

    return -1;    
}

static void disp_menu (INT8U sel)
{
    INT8U i;
	INT8U a,b;
   
    a = sel / 4;
    b = sel % 4;
        
    lcd_clear ();
    bar_init ();

 	roll_pos = sel;
    bar_width_set (roll_cnt);
	bar_pos (sel);
	record_pos[record_layer - 1] = sel;
    
    if (widget_lang_get () == WIDGET_CHINESE)
    {
        for (i = 0; i < 4; i++)
        {
            if (4 * a + i < roll_cnt)
			{
				if (i == b)
	            {
					font_print (0, i * 2, menu_info[roll_order[4 * a + i]].ch_name, 1); 
					current_node = menu_info[roll_order[4 * a + i]].node; 
				}
	            else
	            {
	                font_print (0, i * 2, menu_info[roll_order[4 * a + i]].ch_name, 0);       
	            }
        	}
		}
    }
    else
    {
        for (i = 0; i < 4; i++)
        {
            if (4 * a + i < roll_cnt)
			{
				if (i == b)
	            {
					font_print (0, i * 2, menu_info[roll_order[4 * a + i]].en_name, 1); 
	            	current_node = menu_info[roll_order[4 * a + i]].node;
				}
	            else
	            {
	                font_print (0, i * 2, menu_info[roll_order[4 * a + i]].en_name, 0);       
	            }
        	}           
        }
    }
}

static INT8S enter_1 (INT8U* node, INT8U pos)
{
    INT8U i;
    INT8U node_dot;
    INT8U index = 0;
	INT8U cnt = 0;
     
    if (node == 0)
    {
		//搜索根菜单
        for (i = 0; i < menu_cnt; i++)
        {
            if (get_dot_cnt (menu_info[i].node) == 0)
            {
                roll_order[cnt] = i;
                cnt++;  
            }
        }
        if (cnt == 0)
        {
            return -1;   
        }
        else
        {
            if (back_flag == 1)
            {
                back_flag = 0;
            }
            else
            {
                record_node[record_layer] = current_node;    
            }
            
            roll_cnt = cnt;
            
	       	record_layer++;

			disp_menu (pos); 
          
            return 0; 
        }          
    }
    else
    {
        node_dot = get_dot_cnt (node) + 1;
        
        for (i = 0; i < menu_cnt; i++)
        {
            if (menu_info[i].flag == 1 && strlen (menu_info[i].node) > strlen (node) && get_dot_cnt (menu_info[i].node) == node_dot && is_match (node, menu_info[i].node) == 0)
            {
                roll_order[cnt] = i;
                cnt++;       
            }
            
            if (strcmp (node, menu_info[i].node) == 0)
            {
                index = i;   
            }
        }         
        
        if (cnt == 0)
        { 
            if (menu_info[index].func != 0)
            {
                (*menu_info[index].func) (0);
				
				//恢复
				disp_menu (roll_pos);				

				return 0;      
            }
			else
			{
            	return -1;
			}   
        }
        else
        {
            if (back_flag == 1)
            {
                back_flag = 0;
            }
            else
            {
                record_node[record_layer] = current_node;    
            }
            
            roll_cnt = cnt;
            
	       	record_layer++;

			disp_menu (pos); 
            
            return 0;  
        }       
    }        
}

static void up (void)
{
	if (roll_pos == 0)
	{
		roll_pos = roll_cnt - 1;
	}
	else
	{
		roll_pos--;	
	}
	
	disp_menu (roll_pos);    
}

static void down (void)
{
	if (roll_pos == roll_cnt - 1)
	{
		roll_pos = 0;
	}
	else
	{
		roll_pos++;
	}
	
	disp_menu (roll_pos);
}

static INT8S enter (void)
{
    if (enter_1 (current_node, 0) == 0)
    {
        return 0;
    }
    else
    {
        return -1;   
    }    
}

static INT8S back (void)
{
	if (record_layer == 1)
    {
        return -1;   
    }

    back_flag = 1;

	record_layer -= 2;
	        
    enter_1 (record_node[record_layer], record_pos[record_layer]);

	return 0;
}

static void menu_init (void)
{    
    menu_cnt = sizeof (menu_info) / sizeof (MENU_INFO);
	memset (roll_order, 0, sizeof (roll_order));
    roll_cnt = 0;
    roll_pos = 0;
    memset (record_node, 0, sizeof (record_node));    
    memset (record_pos, 0, sizeof (record_pos)); 
    record_layer = 0;
    current_node = 0;
    back_flag = 0;
}

void menu_main (void* pdata)
{
    //菜单初始化
    menu_init ();
    
    //显示根菜单
    enter ();
    
    //消息循环
    while (1)
    {
		switch (key_msg_get ())
        {
            //上移
            case KEY_UP:
                up (); 
                break;
            
            //下移
            case KEY_DOWN:
                down ();
                break;
            
            //确认
            case KEY_ENTER:
        		if (enter () == -1)      //进入子菜单，或者进入另一控件(如时间设置控件，或者密码控件，<一切皆控件, 控件都有消息循环, 当前控件独享按键消息!!!>)
                {
                    //shell_printf ("end\r\n");   
                }
                break;
                
            //返回    
            case KEY_RETURN:
                if (back () == -1)	 	//进入桌面控件
                {
                    widget_run ("desktop", 0);
                }
                break;
                
            default:
                break;   
        }
    }     
}

/*debug
shell_printf ("layer = %d\r\n", record_layer);
shell_printf ("current_node = %s\r\n", current_node);

shell_printf ("record_node[0] = %s\r\n", record_node[0]);
shell_printf ("record_pos[0] = %d\r\n", record_pos[0]);
shell_printf ("record_node[1] = %s\r\n", record_node[1]);
shell_printf ("record_pos[1] = %d\r\n", record_pos[1]);
shell_printf ("record_node[2] = %s\r\n", record_node[2]);
shell_printf ("record_pos[2] = %d\r\n", record_pos[2]);
*/
