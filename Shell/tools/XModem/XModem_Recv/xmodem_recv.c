#include "includes.h"
#include "xmodem_recv.h"

#define SOH 0x01
#define EOT 0x04
#define ACK 0x06
#define NAK 0x15
#define CAN 0x18 

#define RECV_TIMEOUT 60    //秒	

static INT16U xmodem_send (INT8U* p_data, INT16U data_len)
{	
	return uart_write(STD_UART_PORT, p_data, data_len); 
}

static INT16U xmodem_recv (INT8U* p_data, INT16U data_len, INT16U timeout)
{
	INT16U recvd_data_len;

	uart_ioctl (STD_UART_PORT, UART_UNPEND_RECV, timeout);
	recvd_data_len = uart_read (STD_UART_PORT, p_data, data_len);
	uart_ioctl(STD_UART_PORT, UART_UNPEND_RECV, 0);

	return recvd_data_len;
}

static void xmodem_recv_file (void* pdat)
{
	static INT8U recv_buf[132]; //接收缓存,128 + 4字节
	INT8U ack = NAK;
    INT16U i = 0;
	INT16U len = 0;
	INT8U argc;
	char** argv;
	FILE* f = NULL;
	
	argc = ((SHELL_CMD_PARA*)pdat)->argc;
	argv = ((SHELL_CMD_PARA*)pdat)->argv;

	if (argc == 1)
	{
		shell_printf ("missing file name!");
		shell_return ();
	}
	
	if (argc != 2)
	{
		shell_printf ("parameter error!");
		shell_return ();
	}

	f = fopen (argv[1], "r");

	if (f == NULL)
	{
		f = fopen (argv[1], "w");
	}
	else
	{
		shell_printf ("'%s' already exist!", argv[1]);
		fclose (f);
		shell_return ();	 	
	}

	shell_enter_critical ();
	shell_printf ("xmodem receiving...\r\n");

	//修改 UART_MSG_END_TIME，否则不正常
	uart_ioctl (STD_UART_PORT, UART_FRAME_TIMEOUT, 5);

	while (1)
    {
        xmodem_send (&ack, 1); 

		len = xmodem_recv (recv_buf, sizeof (recv_buf), 50); 

		if (len == 1 && recv_buf[0] == EOT)
		{
			shell_putchar (ACK);
			fclose (f);
			shell_printf (" receive success!"); 
			break; //传输结束
		}
		else if (len == 132)      
        {            
			//校验...
            if (recv_buf[131] != check_sum (recv_buf + 3, 128))
			{
				//请求重发
				ack = NAK;
				continue;
			}
			else
			{
	            //写入文件    
	            fwrite (recv_buf + 3, 1, 128, f); 	
	           	ack = ACK;	//发送应答				
			}

			i = RECV_TIMEOUT * 2 - 10;
        }
        else
        {
            i++;
            
            if (i > RECV_TIMEOUT * 2)
            {
                shell_printf ("receive error!"); 
					
				if (f != NULL)
				{
				 	fclose (f);
					fdelete (argv[1]);
				}

				break;  
            }   
        }   
    }

	//恢复 UART_MSG_END_TIME
    uart_ioctl (STD_UART_PORT, UART_FRAME_TIMEOUT, 1);
	
	shell_exit_critical ();
	shell_return ();
}

void xmodem_recv_file_cmd_reg (void)
{
    shell_cmd_register ("xmdr", xmodem_recv_file, "xmodem protocol receive a file");
}

