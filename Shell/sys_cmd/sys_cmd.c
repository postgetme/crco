#include "includes.h"
#include "sys_cmd.h" 

static void version (void* pdata)
{
	shell_printf("V1.10 ~ 20101218");
	shell_return ();
}

static void help (void* pdata)
{
	extern CMD cmd[CMD_MAX_CNT];

	INT8U i;

	for (i = 0; i < CMD_MAX_CNT; i++)	
   	{
	 	if (cmd[i].str != 0)
		{
			shell_printf (cmd[i].str);
			shell_printf ("  ----------  ");
			shell_printf (cmd[i].info);			 	
			shell_printf ("\r\n");
		}
	}

	shell_return ();
}

static void reboot (void* pdata)
{
	shell_printf("\033[H\033[J");
	shell_disable ();
	SysCtlReset();
	while(1);
}

static void clear (void* pdata)
{
	shell_printf("\033[H\033[J");
	shell_return ();
}

static void date (void* pdata)
{
	TIME time;	
	pcf8563_read (&time);
	shell_printf ("%d年%02d月%02d日 - %02d:%02d:%02d - ", time.year, time.month, time.day, time.hour, time.minute, time.second, time.week);

	switch (time.week)
	{
		case 1:
			shell_printf ("星期一");
			break;
		case 2:
			shell_printf ("星期二");
			break;
		case 3:
			shell_printf ("星期三");
			break;
		case 4:
			shell_printf ("星期四");
			break;
		case 5:
			shell_printf ("星期五");
			break;
		case 6:
			shell_printf ("星期六");
			break;
		case 0:
			shell_printf ("星期日");
			break;								
		default:
			break;
	}

	shell_return ();
}

static void settime (void* pdata)
{
	TIME time;

	INT8U argc;
	char** argv;

	argc = ((SHELL_CMD_PARA*)pdata)->argc;
	argv = ((SHELL_CMD_PARA*)pdata)->argv; 

	if (argc != 8)
	{
		shell_printf ("parameter error!");
		shell_return ();
	}
	
	time.year = atoi (argv[1]);
	time.month = atoi (argv[2]);
	time.day = atoi (argv[3]);
	time.hour = atoi (argv[4]);
	time.minute = atoi (argv[5]);
	time.second = atoi (argv[6]);
	time.week = atoi (argv[7]);
	
	pcf8563_write (&time);

	shell_printf ("set time ok!");

	shell_return ();
} 

void sys_cmd_register (void)
{		
	shell_cmd_register ("help", help, "show command list");
	shell_cmd_register ("clear", clear, "clear screen");
	shell_cmd_register ("cls", clear, "clear screen");		
	shell_cmd_register ("reboot", reboot, "reboot system");
	shell_cmd_register ("date", date, "show current date and time");
	shell_cmd_register ("settime", settime, "set current date and time");
	shell_cmd_register ("ver", version, "show version");
}
