//KEY_2.0 Designed by Mei_Lin at 20101224

#include "includes.h"
#include "key_cfg.h"

#define TYPE_KEY_UP          1
#define TYPE_KEY_DOWN        2
#define TYPE_KEY_DOUBLE      3
#define TYPE_KEY_PUSH        4

static INT32U change;
static void* key_msg_buf[KEY_MSG_Q_SIZE];

static OS_EVENT* os_q_key_msg;
static OS_STK key_scan_task_stk[KEY_SCAN_TASK_STK_SIZE];

#if KEY_DOUBLE_EN == 1
static OS_EVENT* os_sem_double;
static OS_STK key_double_task_stk[KEY_DOUBLE_TASK_STK_SIZE];
#endif

#if KEY_PUSH_EN == 1
static OS_EVENT* os_sem_push;
static OS_STK key_push_task_stk[KEY_PUSH_TASK_STK_SIZE];
#endif

#if KEY_TEST_EN == 1
static OS_STK key_test_task_stk[KEY_TEST_TASK_STK_SIZE];
#endif

extern void key_phy_init (void);
extern INT32U key_phy_read (void);
extern void key_hook_init (void);
extern void key_hook (INT32U msg);

#if KEY_PUSH_EN == 1
static void PushKeyTask (void* pdata)
{
	INT8U err;

	while (1)
	{
		OSSemPend (os_sem_push, PUSH_TIME, &err);
		if (err == OS_TIMEOUT)
		{	
			key_hook (TYPE_KEY_PUSH<<24 | change);
			OSQPost (os_q_key_msg, (void*)(TYPE_KEY_PUSH<<24 | change));
		#if KEY_CONTINUOUS_PUSH_EN == 1
			while (1)	
			{
				key_hook (TYPE_KEY_DOWN<<24 | change);
			    OSQPost (os_q_key_msg, (void*)(TYPE_KEY_PUSH<<24 | change));
				OSTimeDly (PUSH_EXECUTE_INTERVAL);
				if (OSSemAccept(os_sem_push))
				{
					OSSemPost (os_sem_push);
					break;
				}
			}
		#else
			OSTaskDel (KEY_PUSH_TASK_PRIO);
		#endif
		}
		else
		{
			OSTaskDel (KEY_PUSH_TASK_PRIO);
		}
	}
}
#endif

#if KEY_DOUBLE_EN == 1
static void DoubleKeyTask (void* pdata)
{
	INT8U err;

	while (1)
	{
		OSSemPend (os_sem_double, DOUBLE_INTERVAL, &err);
		if (err == OS_TIMEOUT)
		{	
			OSTaskDel (KEY_DOUBLE_TASK_PRIO);
		}
		else
		{
			key_hook (TYPE_KEY_DOUBLE<<24 | change);
			OSQPost (os_q_key_msg, (void*)(TYPE_KEY_DOUBLE<<24 | change));
			OSTaskDel (KEY_DOUBLE_TASK_PRIO);
		}
	}	
}
#endif

static void KeyScan (void* pdata)
{	
	static INT32U backup_read = 0;
	static INT8U judge_flag = 0;
	static INT8U refresh_flag = 0;	
	static INT32U front_read = KEY_DEFAULT_STATE;
    static INT32U current_read = KEY_DEFAULT_STATE;
	INT32U new_read;

#if KEY_PUSH_EN == 1 || KEY_DOUBLE_EN == 1
	static OS_TCB task_data;
#endif
	
	while (1)
	{			
		new_read = key_phy_read () & 0x00ffffff;
			
		if (new_read == backup_read)
		{
			if (refresh_flag == 0)
			{
				front_read = current_read;
				current_read = new_read;				
				
				refresh_flag = 1;
				judge_flag = 1;
			}
		}	
		else
		{
			backup_read = new_read;
			refresh_flag = 0;
		}

		if (judge_flag == 1)
		{		    
			change = front_read ^ current_read;
						
			if (change)
           	{
            	if ((change & current_read) != 0)	
            	{
            	    key_hook (TYPE_KEY_UP<<24 | change);
            	    OSQPost (os_q_key_msg, (void*)(TYPE_KEY_UP<<24 | change));
            	    
            	    #if KEY_PUSH_EN == 1		
                		if (OSTaskQuery (KEY_PUSH_TASK_PRIO, &task_data) == OS_NO_ERR)
                		{
                			OSSemPost (os_sem_push);
                		}
            		#endif
            	}
            	else
            	{
            		key_hook (TYPE_KEY_DOWN<<24 | change);
            		OSQPost (os_q_key_msg, (void*)(TYPE_KEY_DOWN<<24 | change));
            		
            	    #if KEY_PUSH_EN == 1
                		OSTaskCreate (PushKeyTask, (void*)0, &key_push_task_stk[KEY_PUSH_TASK_STK_SIZE - 1], KEY_PUSH_TASK_PRIO);
            		#endif
            		
            		#if KEY_DOUBLE_EN == 1
                   		if (OSTaskQuery (KEY_DOUBLE_TASK_PRIO, &task_data) == OS_NO_ERR)
                		{
                			OSSemPost (os_sem_double);
                		}
                		else
                		{
                		    OSTaskCreate (DoubleKeyTask, (void*)0, &key_double_task_stk[KEY_DOUBLE_TASK_STK_SIZE - 1], KEY_DOUBLE_TASK_PRIO);	
                		}
            		#endif
            	}
		    }
		    		
			judge_flag = 0;
		}

		OSTimeDly (KEY_SCAN_INTERVAL);
	}
}

#if	KEY_TEST_EN == 1
static void KeyTest(void* pdat)
{
	INT32U key = 0;
	
	while (1)
	{
		switch (key = key_msg_get ())
		{
		 	case KEY_1_UP:
				shell_printf ("KEY_1_UP\r\n");
				break;
			case KEY_1_DOWN:
				shell_printf ("KEY_1_DOWN\r\n");
				break;
			case KEY_1_PUSH:
				shell_printf ("KEY_1_PUSH\r\n");
				break;
			case KEY_1_DOUBLE:
				shell_printf ("KEY_1_DOUBLE\r\n");
				break;

		 	case KEY_2_UP:
				shell_printf ("KEY_2_UP\r\n");
				break;
			case KEY_2_DOWN:
				shell_printf ("KEY_2_DOWN\r\n");
				break;
			case KEY_2_PUSH:
				shell_printf ("KEY_2_PUSH\r\n");
				break;
			case KEY_2_DOUBLE:
				shell_printf ("KEY_2_DOUBLE\r\n");
				break;

		 	case KEY_3_UP:
				shell_printf ("KEY_3_UP\r\n");
				break;
			case KEY_3_DOWN:
				shell_printf ("KEY_3_DOWN\r\n");
				break;
			case KEY_3_PUSH:
				shell_printf ("KEY_3_PUSH\r\n");
				break;
			case KEY_3_DOUBLE:
				shell_printf ("KEY_3_DOUBLE\r\n");
				break;

		 	case KEY_4_UP:
				shell_printf ("KEY_4_UP\r\n");
				break;
			case KEY_4_DOWN:
				shell_printf ("KEY_4_DOWN\r\n");
				break;
			case KEY_4_PUSH:
				shell_printf ("KEY_4_PUSH\r\n");
				break;
			case KEY_4_DOUBLE:
				shell_printf ("KEY_4_DOUBLE\r\n");
				break;

		 	case KEY_5_UP:
				shell_printf ("KEY_5_UP\r\n");
				break;
			case KEY_5_DOWN:
				shell_printf ("KEY_5_DOWN\r\n");
				break;
			case KEY_5_PUSH:
				shell_printf ("KEY_5_PUSH\r\n");
				break;
			case KEY_5_DOUBLE:
				shell_printf ("KEY_5_DOUBLE\r\n");
				break;

		 	case KEY_6_UP:
				shell_printf ("KEY_6_UP\r\n");
				break;
			case KEY_6_DOWN:
				shell_printf ("KEY_6_DOWN\r\n");
				break;
			case KEY_6_PUSH:
				shell_printf ("KEY_6_PUSH\r\n");
				break;
			case KEY_6_DOUBLE:
				shell_printf ("KEY_6_DOUBLE\r\n");
				break;

		 	case KEY_7_UP:
				shell_printf ("KEY_7_UP\r\n");
				break;
			case KEY_7_DOWN:
				shell_printf ("KEY_7_DOWN\r\n");
				break;
			case KEY_7_PUSH:
				shell_printf ("KEY_7_PUSH\r\n");
				break;
			case KEY_7_DOUBLE:
				shell_printf ("KEY_7_DOUBLE\r\n");
				break;

		 	case KEY_8_UP:
				shell_printf ("KEY_8_UP\r\n");
				break;
			case KEY_8_DOWN:
				shell_printf ("KEY_8_DOWN\r\n");
				break;
			case KEY_8_PUSH:
				shell_printf ("KEY_8_PUSH\r\n");
				break;
			case KEY_8_DOUBLE:
				shell_printf ("KEY_8_DOUBLE\r\n");
				break;

		 	case KEY_9_UP:
				shell_printf ("KEY_9_UP\r\n");
				break;
			case KEY_9_DOWN:
				shell_printf ("KEY_9_DOWN\r\n");
				break;
			case KEY_9_PUSH:
				shell_printf ("KEY_9_PUSH\r\n");
				break;
			case KEY_9_DOUBLE:
				shell_printf ("KEY_9_DOUBLE\r\n");
				break;

		 	case KEY_10_UP:
				shell_printf ("KEY_10_UP\r\n");
				break;
			case KEY_10_DOWN:
				shell_printf ("KEY_10_DOWN\r\n");
				break;
			case KEY_10_PUSH:
				shell_printf ("KEY_10_PUSH\r\n");
				break;
			case KEY_10_DOUBLE:
				shell_printf ("KEY_10_DOUBLE\r\n");
				break;

		 	case KEY_11_UP:
				shell_printf ("KEY_11_UP\r\n");
				break;
			case KEY_11_DOWN:
				shell_printf ("KEY_11_DOWN\r\n");
				break;
			case KEY_11_PUSH:
				shell_printf ("KEY_11_PUSH\r\n");
				break;
			case KEY_11_DOUBLE:
				shell_printf ("KEY_11_DOUBLE\r\n");
				break;

		 	case KEY_12_UP:
				shell_printf ("KEY_12_UP\r\n");
				break;
			case KEY_12_DOWN:
				shell_printf ("KEY_12_DOWN\r\n");
				break;
			case KEY_12_PUSH:
				shell_printf ("KEY_12_PUSH\r\n");
				break;
			case KEY_12_DOUBLE:
				shell_printf ("KEY_12_DOUBLE\r\n");
				break;

		 	case KEY_13_UP:
				shell_printf ("KEY_13_UP\r\n");
				break;
			case KEY_13_DOWN:
				shell_printf ("KEY_13_DOWN\r\n");
				break;
			case KEY_13_PUSH:
				shell_printf ("KEY_13_PUSH\r\n");
				break;
			case KEY_13_DOUBLE:
				shell_printf ("KEY_13_DOUBLE\r\n");
				break;

		 	case KEY_14_UP:
				shell_printf ("KEY_14_UP\r\n");
				break;
			case KEY_14_DOWN:
				shell_printf ("KEY_14_DOWN\r\n");
				break;
			case KEY_14_PUSH:
				shell_printf ("KEY_14_PUSH\r\n");
				break;
			case KEY_14_DOUBLE:
				shell_printf ("KEY_14_DOUBLE\r\n");
				break;

		 	case KEY_15_UP:
				shell_printf ("KEY_15_UP\r\n");
				break;
			case KEY_15_DOWN:
				shell_printf ("KEY_15_DOWN\r\n");
				break;
			case KEY_15_PUSH:
				shell_printf ("KEY_15_PUSH\r\n");
				break;
			case KEY_15_DOUBLE:
				shell_printf ("KEY_15_DOUBLE\r\n");
				break;

		 	case KEY_16_UP:
				shell_printf ("KEY_16_UP\r\n");
				break;
			case KEY_16_DOWN:
				shell_printf ("KEY_16_DOWN\r\n");
				break;
			case KEY_16_PUSH:
				shell_printf ("KEY_16_PUSH\r\n");
				break;
			case KEY_16_DOUBLE:
				shell_printf ("KEY_16_DOUBLE\r\n");
				break;

		 	case KEY_17_UP:
				shell_printf ("KEY_17_UP\r\n");
				break;
			case KEY_17_DOWN:
				shell_printf ("KEY_17_DOWN\r\n");
				break;
			case KEY_17_PUSH:
				shell_printf ("KEY_17_PUSH\r\n");
				break;
			case KEY_17_DOUBLE:
				shell_printf ("KEY_17_DOUBLE\r\n");
				break;

		 	case KEY_18_UP:
				shell_printf ("KEY_18_UP\r\n");
				break;
			case KEY_18_DOWN:
				shell_printf ("KEY_18_DOWN\r\n");
				break;
			case KEY_18_PUSH:
				shell_printf ("KEY_18_PUSH\r\n");
				break;
			case KEY_18_DOUBLE:
				shell_printf ("KEY_18_DOUBLE\r\n");
				break;

		 	case KEY_19_UP:
				shell_printf ("KEY_19_UP\r\n");
				break;
			case KEY_19_DOWN:
				shell_printf ("KEY_19_DOWN\r\n");
				break;
			case KEY_19_PUSH:
				shell_printf ("KEY_19_PUSH\r\n");
				break;
			case KEY_19_DOUBLE:
				shell_printf ("KEY_19_DOUBLE\r\n");
				break;

		 	case KEY_20_UP:
				shell_printf ("KEY_20_UP\r\n");
				break;
			case KEY_20_DOWN:
				shell_printf ("KEY_20_DOWN\r\n");
				break;
			case KEY_20_PUSH:
				shell_printf ("KEY_20_PUSH\r\n");
				break;
			case KEY_20_DOUBLE:
				shell_printf ("KEY_20_DOUBLE\r\n");
				break;

		 	case KEY_21_UP:
				shell_printf ("KEY_21_UP\r\n");
				break;
			case KEY_21_DOWN:
				shell_printf ("KEY_21_DOWN\r\n");
				break;
			case KEY_21_PUSH:
				shell_printf ("KEY_21_PUSH\r\n");
				break;
			case KEY_21_DOUBLE:
				shell_printf ("KEY_21_DOUBLE\r\n");
				break;

		 	case KEY_22_UP:
				shell_printf ("KEY_22_UP\r\n");
				break;
			case KEY_22_DOWN:
				shell_printf ("KEY_22_DOWN\r\n");
				break;
			case KEY_22_PUSH:
				shell_printf ("KEY_22_PUSH\r\n");
				break;
			case KEY_22_DOUBLE:
				shell_printf ("KEY_22_DOUBLE\r\n");
				break;

		 	case KEY_23_UP:
				shell_printf ("KEY_23_UP\r\n");
				break;
			case KEY_23_DOWN:
				shell_printf ("KEY_23_DOWN\r\n");
				break;
			case KEY_23_PUSH:
				shell_printf ("KEY_23_PUSH\r\n");
				break;
			case KEY_23_DOUBLE:
				shell_printf ("KEY_23_DOUBLE\r\n");
				break;

		 	case KEY_24_UP:
				shell_printf ("KEY_24_UP\r\n");
				break;
			case KEY_24_DOWN:
				shell_printf ("KEY_24_DOWN\r\n");
				break;
			case KEY_24_PUSH:
				shell_printf ("KEY_24_PUSH\r\n");
				break;
			case KEY_24_DOUBLE:
				shell_printf ("KEY_24_DOUBLE\r\n");
				break;

			case 0:
				break;

			default:
				shell_printf ("Undefined key msg %X!\r\n", key);
				break;
		}
	}
}	
#endif

INT8S key_init (void)
{
//  INT8U err;
//	static OS_TMR* os_tmr_scan;    
    key_phy_init ();
    
    #if KEY_HOOK_EN == 1
        key_hook_init ();
    #endif
	    
    //创建扫描任务
//    os_tmr_scan = OSTmrCreate (0, KEY_SCAN_INTERVAL, OS_TMR_OPT_PERIODIC, (OS_TMR_CALLBACK)KeyScan, 0, 0, &err);
//    if (err != OS_ERR_NONE)
//    {
//        return -1;      
//    }
//    OSTmrStart (os_tmr_scan, &err);

	OSTaskCreate (KeyScan, (void*)0, &key_scan_task_stk[KEY_SCAN_TASK_STK_SIZE - 1], KEY_SCAN_TASK_PRIO);

	os_q_key_msg = OSQCreate (key_msg_buf, KEY_MSG_Q_SIZE);
    
    #if KEY_DOUBLE_EN == 1
        os_sem_double = OSSemCreate (0); 
    #endif
    
    #if KEY_PUSH_EN == 1
        os_sem_push = OSSemCreate (0);
    #endif

    #if KEY_TEST_EN == 1
        OSTaskCreate (KeyTest, (void*)0, &key_test_task_stk[KEY_TEST_TASK_STK_SIZE - 1], KEY_TEST_TASK_PRIO);
    #endif

	return 0;
}

INT32U key_msg_get (void)
{
	INT8U err;

	return (INT32U)OSQPend (os_q_key_msg, KEY_MSG_TIMEOUT, &err);	
}

INT8S key_msg_post (INT32U msg)
{
    #if KEY_HOOK_EN == 1
        key_hook (msg);
    #endif
    
    return OSQPost (os_q_key_msg, (void*)msg) == OS_NO_ERR ? 0 : -1;
}

void key_msg_flush (void)
{
    OSQFlush (os_q_key_msg);   
}
