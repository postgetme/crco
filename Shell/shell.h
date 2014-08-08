#ifndef SHELL_H_H
#define SHELL_H_H

/*���Ŀ��
1.֧���������
2.֧��tab���ȫ
3.Ӧ�ó���ӿں���
shell_read()
shell_write()
shell_cmd_rigister()
shell_return();
֧��system����????????????????????????????????????????
�û����򰴼�����??????????????????????????????????????								 
4.֧��ɱ����������

shell����
1.com����
2.xmodem���ͽ���
3.calc
4.IO����

֧�ֵ�ϵͳ����
clear,reboot,help

�ļ���������
ls,cat,copy,rename,del

δ��֧��,vi�ı��༭,./ִ�нű��ļ�*/

//�д����Ƶĵط�,1.ע������ʱ������������. 2.���������ļ�, �ɽ�ֹ�������,�粻֧�ִ��ڹ���

/************** �����ò��� *******************/
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
