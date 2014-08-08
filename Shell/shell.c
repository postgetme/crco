#include "includes.h"
#include "sys_cmd/sys_cmd.h"
#include "fs_cmd/fs_cmd.h"
#include "tools/Xmodem/XModem_Recv/xmodem_recv.h"
#include "tools/com_tool/com_tool.h"
#include "tools/io_tool/io_tool.h"

#define KB_UP           0x415b1b
#define KB_DOWN         0x425b1b
#define KB_LEFT         0x445b1b
#define KB_RIGHT        0x435b1b
#define KB_F1           0x504f1b
#define KB_F2           0x514f1b
#define KB_F3           0x524f1b
#define KB_F4           0x534f1b
#define KB_ENTER        0x0d
#define KB_TAB          0x09
#define KB_DEL          0x7F
#define KB_BACKSPACE    0x08
#define KB_ESC          0x1B
#define KB_SPACE        0x20
#define KB_CTRL_C		0x03 

#if SHELL_MAIN_TASK_PRIO >= SHELL_CMD_TASK_PRIO
#error "SHELL_MAIN_TAST_PRIO must < SHELL_CMD_TASK_PRIO"
#endif	

typedef	struct
{
	char* buf;
	INT16U size;
}USER_READ_DATA;

CMD cmd[CMD_MAX_CNT];
static SHELL_CMD_PARA para;
static OS_STK cmd_task_stk[SHELL_CMD_TASK_STK_SIZE];
static OS_STK main_task_stk[SHELL_MAIN_TASK_STK_SIZE];
static char line_word[LINE_WORD_MAX_CNT][LINE_WORD_MAX_LEN];
static OS_EVENT* user_read_mbox;

void shell_disable (void)
{
	#if STD_UART_PORT == 0
		UARTDisable(UART0_BASE);
	#endif

	#if STD_UART_PORT == 1
		UARTDisable(UART1_BASE);
	#endif

	#if STD_UART_PORT == 2
		UARTDisable(UART2_BASE);
	#endif
}

static void stdio_init(void)
{
	uart_init(STD_UART_PORT, STD_UART_BPS, 8, 0 ,1, SHELL_UART_485_CTL_IO);
}

static void stdio_write(INT8U* buf, INT16U size)
{
    uart_write(STD_UART_PORT, buf, size);
}

static void stdio_putchar(char ch)
{
    stdio_write((INT8U*)&ch, 1);
}

static void stdio_printf(char* fmt, ...)
{
    va_list argptr;
    static char s[200];
    INT8U len;

    va_start(argptr, fmt);
    len = vsnprintf(s, sizeof(s), fmt, argptr);
    va_end(argptr);

	stdio_write((INT8U*)s, len);
}

static INT16U stdio_read(INT8U* buf, INT16U size)
{
    return uart_read(STD_UART_PORT, buf, size);
}
/*
static void cursor_move_left (void)
{
	stdio_putchar(0x1b);
	stdio_putchar(0x5b);
	stdio_putchar(0x44);
}

static void cursor_move_right (void)
{
	stdio_putchar(0x1b);
	stdio_putchar(0x5b);
	stdio_putchar(0x43);
}
*/
static void del_front(void)
{
    stdio_putchar(KB_BACKSPACE);
    stdio_putchar(KB_SPACE);
    stdio_putchar(KB_BACKSPACE);
}
/*
static void del_back(void)
{
	cursor_move_right ();
	del_front ();
}
*/
static INT16U insert_char (char* buf, INT16U pos, char ch)
{
    buf[pos] = ch;
    
    return pos;
}

void shell_return (void)
{
	stdio_printf (SHELL_PROMPT);
	OSTaskDel (SHELL_CMD_TASK_PRIO);
}

static INT16U line_edit(char* buf, INT16U size)
{
    INT16U strlen = 0;
    INT8U kb_buf[4];
    INT8U len = 0;
    INT32U kb_val = 0;
	INT16U cursor = 0;
	USER_READ_DATA user_read_data;
	OS_MBOX_DATA mbox_state;
	OS_TCB task_state;

    while (1)
    {
        memset (kb_buf, 0, 4);
		len = stdio_read (kb_buf, 4);
        if (len > 0)
        {
			kb_val = *(INT32U*)kb_buf;
			
			/***********防止命令执行过程中键盘输入**********/
			if (OSTaskQuery (SHELL_CMD_TASK_PRIO, &task_state) == OS_NO_ERR && 
				kb_val != KB_CTRL_C &&			
				(OSMboxQuery (user_read_mbox, &mbox_state), (mbox_state.OSEventTbl[SHELL_CMD_TASK_PRIO / 8] & (1<<SHELL_CMD_TASK_PRIO % 8)) == 0))
			{
				continue;
			}
			/*************************************************/

            switch (kb_val)
            {
                /*向前删除一个字符*/
                case KB_BACKSPACE:
					if (strlen > 0 && cursor > 0)
					{
						strlen--;
						cursor--;
						del_front();
					}
                    break;
                /*向后删除一个字符*/
                case KB_DEL:
                    break;

                /*自动补全*/
                case KB_TAB:
                    break;
                /*上一次的命令*/
                case KB_UP:
                    break;
                /*下一次的命令*/
                case KB_DOWN:
                    break;

                /*光标移动*/
                case KB_LEFT:
                    break;
                case KB_RIGHT:
                    break;

                /*重新编辑*/
                case KB_ESC:
					while (strlen)
					{
					 	del_front ();
						strlen--;
					}
					cursor = 0;
                    break;

                /*编辑完成*/
                case KB_ENTER:
					if (OSMboxQuery (user_read_mbox, &mbox_state), mbox_state.OSEventTbl[SHELL_CMD_TASK_PRIO / 8] & (1<<SHELL_CMD_TASK_PRIO % 8))
                   	{
						user_read_data.buf = buf;
						user_read_data.size = strlen;
						OSMboxPost (user_read_mbox, (void*)&user_read_data);

						strlen = 0;
						cursor = 0;

						break;
					}
					else
					{
						return strlen;
					}
                /*终止命令执行*/
				case KB_CTRL_C:
					if (OSTaskQuery (SHELL_CMD_TASK_PRIO, &task_state) == OS_NO_ERR)
					{
						shell_return ();
					}
					break;
                /*输入可见字符*/
                default:
                	if (len == 1 && kb_buf[0] >= 0x20 && kb_buf[0] <= 0x7e && strlen < size)
            		{
						stdio_write (kb_buf, 1);
						insert_char (buf, cursor, kb_buf[0]);
						strlen++;
						cursor++;
            		}
                    break;
            }
        }
    }
}

static INT8U line_trim_space (char* buf, INT16U size)
{
	INT8U i = 0;
	INT8U j = 0;
	INT8U k = 0;

	memset (line_word, 0, LINE_WORD_MAX_CNT * LINE_WORD_MAX_LEN);

	for (i = 0; i < size;)
	{
		if (buf[i] == ' ')
		{
			i++;
		}
		else
		{
			if (j < LINE_WORD_MAX_LEN)
			{
				if (k < LINE_WORD_MAX_CNT)
				{
					line_word[k][j] = buf[i];
					i++;
					j++;
				}
				else
				{
					stdio_printf ("\r\nError! Words count must <= %d!", LINE_WORD_MAX_CNT);
					return 0;
				}
			}
			else
			{
				stdio_printf ("\r\nError! Words length must <= %d!", LINE_WORD_MAX_LEN);
				return 0;
			}

			if (buf[i] == ' ' && i < size)
			{
				k++;
				j = 0;
			}
		}
	}

	if (buf[size - 1] == ' ')
	{
		return k;
	}
	else
	{
		return k + 1;
	}
}

static INT8S line_exe (char* buf, INT16U size)
{
	INT8U i, j;
	INT8U cnt;
	static char* argv[LINE_WORD_MAX_CNT];

	cnt = line_trim_space (buf, size);

	if (cnt == 0)
	{
	 	return -1;
	}

	for (i = 0; i < CMD_MAX_CNT; i++)
	{
		if (strcmp (cmd[i].str, line_word[0]) == 0)
		{
			para.argc = cnt;

			for (j = 0; j < cnt; j++)
			{
			 	argv[j] = line_word[j];
			}
			para.argv = argv;
			stdio_printf ("\r\n");
			OSTaskCreate ((void (*)(void*))cmd[i].func, (void*)&para, &cmd_task_stk[SHELL_CMD_TASK_STK_SIZE - 1], SHELL_CMD_TASK_PRIO);
			return 0;
		}
	}

	return -1;
}

INT8S shell_cmd_register (char* str, void(*func)(void* pdata), char* info)
{
	static INT8U cnt = 0;

	if (cnt >= CMD_MAX_CNT)
	{
		return -1;
	}
	else
	{
	 	cmd[cnt].str = str;
		cmd[cnt].func = func;
		cmd[cnt].info = info;
		cnt++;

		return 0;
	}
}

void shell_write (INT8U* buf, INT16U size)
{
    stdio_write (buf, size);
}

void shell_putchar(char ch)
{
    if (ch == '\n')
	{
	 	shell_putchar ('\r');	
	}
	
	shell_write ((INT8U*)&ch, 1);
}

void shell_printf(char* fmt, ...)
{
    va_list argptr;
    static char s[SHELL_PRINTF_BUF_SIZE];
    INT8U len;

    va_start (argptr, fmt);
    len = vsnprintf (s, sizeof (s), fmt, argptr);
    va_end (argptr);

	shell_write ((INT8U*)s, len);
}

INT16U shell_read (INT8U* buf, INT16U size)
{
	INT8U err;
	USER_READ_DATA* pdata;

	pdata = (USER_READ_DATA*)OSMboxPend (user_read_mbox, 0, &err);

	if (pdata->size < size)
	{
	 	memcpy (buf, pdata->buf, pdata->size);
		return pdata->size;
	}
	else
	{
		memcpy (buf, pdata->buf, size);
		return size;
	}
}

void shell_enter_critical (void)
{
	OSTaskSuspend (SHELL_MAIN_TASK_PRIO);
}

void shell_exit_critical (void)
{
	OSTaskResume (SHELL_MAIN_TASK_PRIO);
}

static void shell_main (void* pdata)
{
	static char buf[LINE_CHAR_MAX_CNT];
	INT16U len;
	
	stdio_printf (SHELL_DECLARE);
	stdio_printf (SHELL_PROMPT);
	
	while(1)
	{
		len = line_edit (buf, sizeof (buf));
		if (len > 0)
		{
			if (line_exe (buf, len) != 0)
			{
				stdio_printf ("\r\nBad command!!!");
				stdio_printf (SHELL_PROMPT);
			}
		}
		else if (len == 0)
		{
			stdio_printf (SHELL_PROMPT);
		}
	}
}

void shell_init (void)
{
	//变量初始化
	user_read_mbox = OSMboxCreate ((void*)0);
	memset ((char*)cmd, 0, sizeof (CMD) * CMD_MAX_CNT);

   	//其它初始化
	stdio_init ();
	sys_cmd_register ();
	
	#if FS_EN == 1
		fs_cmd_register ();	
		xmodem_recv_file_cmd_reg ();
	#endif

	com_cmd_reg ();  
	io_cmd_reg ();  
	
	//创建shell主线程
	OSTaskCreate (shell_main, (void*)0, &main_task_stk[SHELL_MAIN_TASK_STK_SIZE - 1], SHELL_MAIN_TASK_PRIO);
}
