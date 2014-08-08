//********************* 系统配置 *********************
#define JTAG_LOCK 								0

#define SHELL_EN								1
#define SHELL_UART_PORT						   	0
#define SHELL_UART_BPS							115200
#define SHELL_UART_485_CTL_IO					""

#define FS_EN									1

#define KEY_EN									1
#define RESET_KEY_EN							1

#define GUI_EN									0

#define TCP_EN									1

#define CHIP_WD_EN								0

#define RTC_EN									1

//********************* 系统任务优先级和堆栈大小定义 **********************
#define MAIN_TASK_PRIO							2

#define SHELL_MAIN_TASK_PRIO					3
#define SHELL_CMD_TASK_PRIO						4

#define KEY_SCAN_TASK_PRIO 						5
#define KEY_DOUBLE_TASK_PRIO					6
#define KEY_PUSH_TASK_PRIO						7  //KEY_PUSH_TASK_PRIO 一定要大于 KEY_DOUBLE_TASK_PRIO
#define KEY_HOOK_TASK_PRIO						8
#define KEY_TEST_TASK_PRIO						9

#define RESET_KEY_TASK1_PRIO					10
#define RESET_KEY_TASK2_PRIO					11

#define WIDGET_MANAGE_TASK_PRIO					12
#define WIDGET_TASK_PRIO 						13

#define	RG_LED_TASK1_PRIO						14

#define MAIN_TASK_STK_SIZE						100

#define SHELL_MAIN_TASK_STK_SIZE				100
#define SHELL_CMD_TASK_STK_SIZE					300

#define KEY_SCAN_TASK_STK_SIZE					200
#define KEY_DOUBLE_TASK_STK_SIZE				100
#define KEY_PUSH_TASK_STK_SIZE					100
#define KEY_HOOK_TASK_STK_SIZE					100
#define KEY_TEST_TASK_STK_SIZE					100

#define RESET_KEY_TASK1_STK_SIZE				32
#define RESET_KEY_TASK2_STK_SIZE				32

#define WIDGET_MANAGE_TASK_STK_SIZE				100
#define WIDGET_TASK_STK_SIZE					200

#define	RG_LED_TASK1_STK_SIZE				   	32

//********************* 用户任务优先级和堆栈大小定义 **********************
#define APP_TASK0_PRIO							25
#define APP_TASK1_PRIO							16
#define APP_TASK2_PRIO							17
#define APP_TASK3_PRIO							18
#define APP_TASK4_PRIO							19
#define APP_TASK5_PRIO							20
												
#define DESKTOP_TASK1_PRIO						21
#define DESKTOP_TASK2_PRIO						22

#define APP_TASK0_STK_SIZE						100
#define APP_TASK1_STK_SIZE						100
#define APP_TASK2_STK_SIZE						100
#define APP_TASK3_STK_SIZE						100
#define APP_TASK4_STK_SIZE						100
#define APP_TASK5_STK_SIZE						100

#define DESKTOP_TASK1_STK_SIZE					100
#define DESKTOP_TASK2_STK_SIZE					100


