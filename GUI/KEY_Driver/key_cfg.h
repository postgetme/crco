//KEY_2.0 Designed by Mei_Lin at 20101224

#ifndef KEY_CFG_H_H
#define KEY_CFG_H_H	

//**************************************************************
//注意,要保证DOUBLE_INTERVAL < PUSH_TIME，否则不能区分双击和长按
//**************************************************************

//***********************************************************************
//注意,要保证DOUBLE_TASK_PRIO < PUSH_TASK_PRIO,否则有问题,忘了是什么问题?
//***********************************************************************

//双击键配置
#define KEY_DOUBLE_EN                   0		//是否产生double消息，禁止double可减少一个任务
#define DOUBLE_INTERVAL                 50  	//双击键的间隔时间，单位是OS_TICK
//#define KEY_DOUBLE_TASK_STK_SIZE        100
//#define KEY_DOUBLE_TASK_PRIO            7

//长按键配置
#define KEY_PUSH_EN                     0		//是否产生push消息，禁止push也可减少一个任务
#define KEY_CONTINUOUS_PUSH_EN			1		//是否允许长按之后连续产生DOWN消息
#define PUSH_TIME                       100	    //长按键的长按时间，单位是OS_TICK
#define PUSH_EXECUTE_INTERVAL           30	    //长按键任务的执行间隔，单位是OS_TICK
//#define KEY_PUSH_TASK_STK_SIZE          100
//#define KEY_PUSH_TASK_PRIO              (DOUBLE_TASK_PRIO + 1)

//全局配置 
#define KEY_DEFAULT_STATE               0x000001//定义按键初始值
#define KEY_SCAN_INTERVAL               5		//按键扫描间隔，单位是OS_TICK
#define KEY_MSG_TIMEOUT					20		//读取按键消息的超时时间，为0则阻塞，单位是OS_TICK
#define KEY_MSG_Q_SIZE                  10		//按键队列大小
#define KEY_HOOK_EN                     0
//#define KEY_SCAN_TASK_PRIO 				6
//#define KEY_SCAN_TASK_STK_SIZE 			300
#define KEY_TEST_EN						0
//#define KEY_TEST_TASK_PRIO			9
//#define KEY_TEST_TASK_STK_SIZE		100

#endif
