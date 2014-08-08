#include "includes.h"
#include "com_tool.h"

#define COM_TOOL_RECV_TASK_PRIO 21
#define COM_TOOL_RECV_TASK_STK_SIZE 100

static OS_STK recv_task_stk[COM_TOOL_RECV_TASK_STK_SIZE]; 
INT8U port = 0;	 
INT8U mode = 0;	

static void recv (void* pdata)
{
	OS_TCB task_state;

	static INT8U buf[400];
	static INT8U buf2[200];
	INT16U len;
	INT16U len2;
	
	uart_ioctl (port, UART_UNPEND_RECV, 50); 
	
	while (1)
	{
		len = uart_read (port, buf, sizeof (buf));
		
		//判断命令线程是否被删除, 没有删除则正常执行,否则将自己删除
		if (OSTaskQuery (SHELL_CMD_TASK_PRIO, &task_state) == OS_NO_ERR)   
		{
			if (len > 0)
			{
				if (mode == 0)
				{
					shell_printf ("\r\n<- ");	
					shell_write (buf, len);
					shell_printf ("\r\n-> ");	
				}
				else
				{
					buf[len] = 0;	
					len2 = asc_to_hex ((char*)buf, (char*)buf2);
	
					shell_printf ("\r\n<- ");	
					shell_write (buf2, len2);
					shell_printf ("\r\n-> ");	
				}
			}
		}
		else
		{
			switch (port)
			{
			 	case 0:
					UARTDisable(UART0_BASE);
					break;
	
				case 1:
					UARTDisable(UART1_BASE);
					break;
	
				case 2:
					UARTDisable(UART2_BASE);
					break;
	
				default:
					break;
			}

			uart_ioctl (port, UART_UNPEND_RECV, 0); 

			OSTaskDel (OS_PRIO_SELF);
		}
	}
}

//格式: com PORT BPS MODE
static void com (void* pdat)
{
	INT8U argc;
	char** argv;
	static INT8U buf[400];
	static INT8U buf2[200];
	INT16U len;
	INT16U len2;
		
	argc = ((SHELL_CMD_PARA*)pdat)->argc;
	argv = ((SHELL_CMD_PARA*)pdat)->argv;
	
	if (argc != 4)
	{
		shell_printf ("parameter error!");
		shell_return ();
	}

	port = atoi (argv[1]);
	
	if (port == STD_UART_PORT)
	{
		shell_printf ("com%s already used for std port!", argv[1]);
		shell_return ();
	}

	if (!(strcmp (argv[3], "hex") == 0 || strcmp (argv[3], "asc") == 0))
	{
		shell_printf ("MODE must be asc/hex!");
		shell_return ();
	}

	if (strcmp (argv[3], "hex") == 0)
	{
		mode = 1;
	}
	else 
	{
	 	mode = 0;
	}
	
	//初始化串口
	if (uart_init (port, atoi (argv[2]), 8, 0 ,1, "") != 0)
	{
		shell_printf ("com init fail!");
		shell_return ();
	}
	else
	{
	 	shell_printf ("open com%s_%s_%s success!", argv[1], argv[2], argv[3]);
	}

	//创建接收任务
	OSTaskCreate (recv, (void*)0, &recv_task_stk[COM_TOOL_RECV_TASK_STK_SIZE - 1], COM_TOOL_RECV_TASK_PRIO);

	//处理数据发送
	while (1)
    {
		shell_printf ("\r\n-> ");

		len = shell_read ((INT8U*)buf, sizeof (buf));
		
		//文本模式
		if (mode == 0)
		{
			uart_write (port, buf, len);
		}
		//十六进制模式
		else
		{
			buf[len] = 0;	
			len2 = hex_to_asc ((char*)buf, (char*)buf2);
			uart_write (port, buf2, len2);
		}
    }
}

void com_cmd_reg (void)
{
    shell_cmd_register ("com", com, "a com tool, it format is 'com PORT BPS MODE'");
}

