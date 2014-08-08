#include "includes.h"

#define WIDGET_MAX_CNT          10

typedef struct
{
    INT8U* widget_name;
    void (*widget_main) (void* pdata);
    void (*widget_destroy) (void);
    INT8U widget_run_flag;
}WIDGET_INFO;

static WIDGET_INFO widget_info[WIDGET_MAX_CNT];

__align(8) static OS_STK widget_manage_task_stk[WIDGET_MANAGE_TASK_STK_SIZE];
__align(8) static OS_STK widget_task_stk[WIDGET_TASK_STK_SIZE];

static INT8U* g_widget_name;
static void* g_widget_pdata;
static OS_EVENT* os_sem_widget_run;
static void* g_sub_ret;

static INT8U widget_lang = WIDGET_CHINESE;

INT8S widget_register (INT8U* widget_name, void (*widget_main) (void* pdata), void (*widget_destroy) (void))
{
	static INT8U cnt = 0;

	if (cnt >= WIDGET_MAX_CNT || widget_name == 0 || widget_main == 0)
	{
		return -1;
	}
	else
	{
	 	widget_info[cnt].widget_name = widget_name;
		widget_info[cnt].widget_main = widget_main;
		widget_info[cnt].widget_destroy = widget_destroy;
		
		cnt++;

		return 0;
	}    
}

static INT8S widget_run_1 (INT8U* widget_name, void* pdata)
{
	INT8U i;
	INT8U index1 = 0xff;
	INT8U index2 = 0xff;
	
	//�жϿؼ��Ƿ�ע��
	for (i = 0; i < WIDGET_MAX_CNT; i++)
	{
		if (strcmp (widget_info[i].widget_name, widget_name) == 0)
		{
            index1 = i;
			break;
		}
	}
		
	if (index1 == 0xff)
	{
	    return -1;   
	}   
    
    //�жϿؼ��Ƿ��������У������ظ�����ͬһ�ؼ����������Ҫ�ظ�����ͬһ�ؼ��Ļ����˴����������޸�    
    if (widget_info[index1].widget_run_flag == 1)
    {
        return -1;   
    }
    
    //���пؼ�
    {        
        //ɾ����ǰ�ؼ�����
        OSTaskDel (WIDGET_TASK_PRIO);
        
        //���õ�ǰ�ؼ���������
        for (i = 0; i < WIDGET_MAX_CNT; i++)
    	{
    		if (widget_info[i].widget_run_flag == 1)
    		{
                index2 = i;
    			break;
    		}
    	}    	
    	
    	if (index2 == 0xff)//������״οؼ�����,��������
    	{
    	    ;
    	}
    	else
    	{
            if (widget_info[index2].widget_destroy != 0)
            {
                (*widget_info[index2].widget_destroy) ();
            }
            
            widget_info[index2].widget_run_flag = 0;     	    
    	}
    	
        //�ÿؼ����б�־
        widget_info[index1].widget_run_flag = 1; 
        
        //��յ�ǰ�ؼ��İ�����Ϣ����
        key_msg_flush ();
        
        //�����µĿؼ�����        
        OSTaskCreate (widget_info[index1].widget_main, pdata, &widget_task_stk[WIDGET_TASK_STK_SIZE - 1], WIDGET_TASK_PRIO);
    }
    
    return 0;   
}

void widget_lang_set (INT8U lang)
{
    if (lang == WIDGET_CHINESE || lang == WIDGET_ENGLISH)
    {
        widget_lang = lang; 
    }       
}

INT8U widget_lang_get (void)
{
    return widget_lang;
}

INT8S widget_run (INT8U* widget_name, void* pdata)
{
	g_widget_name = widget_name;
	g_widget_pdata = pdata;

	OSSemPost (os_sem_widget_run);

	return 0;
}

static void widget_manage_task (void* pdata)
{
	INT8U err;
	
	while (1)
	{
		OSSemPend (os_sem_widget_run, 0, &err);
		widget_run_1 (g_widget_name, g_widget_pdata);
	}
}

void widget_init (void)
{
    memset (widget_info, sizeof (widget_info), 0);

	os_sem_widget_run = OSSemCreate (0);
	
	OSTaskCreate (widget_manage_task, 0, &widget_manage_task_stk[WIDGET_MANAGE_TASK_STK_SIZE - 1], WIDGET_MANAGE_TASK_PRIO);    
}

void* widget_enter_sub (INT8U* widget_name, void* pdata)
{
	INT8U i;
	INT8U index1 = 0xff;
	
	//�жϿؼ��Ƿ�ע��
	for (i = 0; i < WIDGET_MAX_CNT; i++)
	{
		if (strcmp (widget_info[i].widget_name, widget_name) == 0)
		{
            index1 = i;
			break;
		}
	}
		
	if (index1 == 0xff)
	{
	    return 0;   
	} 

	(*widget_info[index1].widget_main) (pdata);

	return g_sub_ret;
}

INT8S widget_exit_sub (void* pdata)
{
	g_sub_ret = pdata;	

	return 0;
}

