//KEY_2.0 Designed by Mei_Lin at 20101224

#ifndef KEY_CFG_H_H
#define KEY_CFG_H_H	

//**************************************************************
//ע��,Ҫ��֤DOUBLE_INTERVAL < PUSH_TIME������������˫���ͳ���
//**************************************************************

//***********************************************************************
//ע��,Ҫ��֤DOUBLE_TASK_PRIO < PUSH_TASK_PRIO,����������,������ʲô����?
//***********************************************************************

//˫��������
#define KEY_DOUBLE_EN                   0		//�Ƿ����double��Ϣ����ֹdouble�ɼ���һ������
#define DOUBLE_INTERVAL                 50  	//˫�����ļ��ʱ�䣬��λ��OS_TICK
//#define KEY_DOUBLE_TASK_STK_SIZE        100
//#define KEY_DOUBLE_TASK_PRIO            7

//����������
#define KEY_PUSH_EN                     0		//�Ƿ����push��Ϣ����ֹpushҲ�ɼ���һ������
#define KEY_CONTINUOUS_PUSH_EN			1		//�Ƿ�������֮����������DOWN��Ϣ
#define PUSH_TIME                       100	    //�������ĳ���ʱ�䣬��λ��OS_TICK
#define PUSH_EXECUTE_INTERVAL           30	    //�����������ִ�м������λ��OS_TICK
//#define KEY_PUSH_TASK_STK_SIZE          100
//#define KEY_PUSH_TASK_PRIO              (DOUBLE_TASK_PRIO + 1)

//ȫ������ 
#define KEY_DEFAULT_STATE               0x000001//���尴����ʼֵ
#define KEY_SCAN_INTERVAL               5		//����ɨ��������λ��OS_TICK
#define KEY_MSG_TIMEOUT					20		//��ȡ������Ϣ�ĳ�ʱʱ�䣬Ϊ0����������λ��OS_TICK
#define KEY_MSG_Q_SIZE                  10		//�������д�С
#define KEY_HOOK_EN                     0
//#define KEY_SCAN_TASK_PRIO 				6
//#define KEY_SCAN_TASK_STK_SIZE 			300
#define KEY_TEST_EN						0
//#define KEY_TEST_TASK_PRIO			9
//#define KEY_TEST_TASK_STK_SIZE		100

#endif
