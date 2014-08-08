#ifndef SHELL_H_H
#define SHELL_H_H

/*设计目标
1.支持命令记忆
2.支持tab命令补全
3.应用程序接口函数
shell_read()
shell_write()
shell_cmd_rigister()
shell_return();
支持system命令????????????????????????????????????????
用户程序按键捕获??????????????????????????????????????								 
4.支持杀死命令任务

shell工具
1.com工具
2.xmodem发送接收
3.calc
4.IO工具

支持的系统命令
clear,reboot,help

文件操作命令
ls,cat,copy,rename,del

未来支持,vi文本编辑,./执行脚本文件*/

//有待完善的地方,1.注册命令时增加析构函数. 2.增加配置文件, 可禁止相关命令,如不支持串口工具

/************** 可配置参数 *******************/
#define STD_UART_PORT SHELL_UART_PORT
#define STD_UART_BPS SHELL_UART_BPS

#define SHELL_DECLARE "\r\n\r\n*******************************\r\n          APPLICATION\r\n       MEILIN_2010.10.22\r\n*******************************\r\n"
#define SHELL_PROMPT "\r\nSHELL\\> "

#define CMD_MAX_CNT 50

#define LINE_CHAR_MAX_CNT 100
#define LINE_WORD_MAX_CNT 20
#define LINE_WORD_MAX_LEN 30
#define SHELL_PRINTF_BUF_SIZE 2048

/*********************************************/
typedef struct
{
	INT8U argc;
	char** argv;
}SHELL_CMD_PARA;

typedef struct
{
	char* str;
	void(*func)(void* pdata);
	char* info;
}CMD;
	
extern void shell_init (void);
extern INT8S shell_cmd_register (char* str, void(*func)(void* pdata), char* info);
extern void shell_return (void);

extern void shell_disable (void);

extern void shell_enter_critical (void);
extern void shell_exit_critical (void);

//extern INT8S system (char* str);

extern INT16U shell_read (INT8U* buf, INT16U size);
extern void shell_write (INT8U* buf, INT16U size);
extern void shell_putchar(char ch);
extern void shell_printf(char* fmt, ...);

#endif
