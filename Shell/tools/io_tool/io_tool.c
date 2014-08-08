#include "includes.h"
#include "io_tool.h"

/*参考资料:
#define GPIO_PIN_0              0x00000001  // GPIO pin 0
#define GPIO_PIN_1              0x00000002  // GPIO pin 1
#define GPIO_PIN_2              0x00000004  // GPIO pin 2
#define GPIO_PIN_3              0x00000008  // GPIO pin 3
#define GPIO_PIN_4              0x00000010  // GPIO pin 4
#define GPIO_PIN_5              0x00000020  // GPIO pin 5
#define GPIO_PIN_6              0x00000040  // GPIO pin 6
#define GPIO_PIN_7              0x00000080  // GPIO pin 7

#define SYSCTL_PERIPH_GPIOA     0x20000001  // GPIO A
#define SYSCTL_PERIPH_GPIOB     0x20000002  // GPIO B
#define SYSCTL_PERIPH_GPIOC     0x20000004  // GPIO C
#define SYSCTL_PERIPH_GPIOD     0x20000008  // GPIO D
#define SYSCTL_PERIPH_GPIOE     0x20000010  // GPIO E
#define SYSCTL_PERIPH_GPIOF     0x20000020  // GPIO F
#define SYSCTL_PERIPH_GPIOG     0x20000040  // GPIO G
#define SYSCTL_PERIPH_GPIOH     0x20000080  // GPIO H

#define GPIO_PORTA_BASE         0x40004000  // GPIO Port A
#define GPIO_PORTB_BASE         0x40005000  // GPIO Port B
#define GPIO_PORTC_BASE         0x40006000  // GPIO Port C
#define GPIO_PORTD_BASE         0x40007000  // GPIO Port D

#define GPIO_PORTE_BASE         0x40024000  // GPIO Port E
#define GPIO_PORTF_BASE         0x40025000  // GPIO Port F
#define GPIO_PORTG_BASE         0x40026000  // GPIO Port G
#define GPIO_PORTH_BASE         0x40027000  // GPIO Port H
*/

/* operations
OUT MODE:

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
	GPIODirModeSet(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_DIR_MODE_OUT);

	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 0);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, GPIO_PIN_1);

IN MODE:
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
	GPIOPadConfigSet(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
	GPIODirModeSet(GPIO_PORTD_BASE, GPIO_PIN_1, GPIO_DIR_MODE_IN);

	state = GPIOPinRead(GPIO_PORTD_BASE,GPIO_PIN_1);			
*/

//格式: io PORT MODE
static void io (void* pdat)
{
	INT8U argc;
	char** argv;
	
	INT8U mode = 0;
	INT32U pin = 0;
	INT32U periph = 0;
	INT32U base = 0;

	INT8U state = 0;
	INT8U backup_state = 0xff;

	INT32U times = 0;

	static INT8U buf[5];
	INT16U len;
		
	argc = ((SHELL_CMD_PARA*)pdat)->argc;
	argv = ((SHELL_CMD_PARA*)pdat)->argv;

	if (argc != 3)
	{
		shell_printf ("parameter error!");
		shell_return ();
	}

	if (!(argv[1][0] == 'p' && 
		(argv[1][1] >= 'a' && argv[1][1] <= 'h') &&
		(argv[1][2] >= '0' && argv[1][2] <= '7') &&
		argv[1][3] == 0))
	{
		shell_printf ("PORT must be pa0~7 ~ ph0~7!");
		shell_return ();
	}
	else
	{
		pin = 1 << (argv[1][2] - '0');
		periph = (1 << (argv[1][1] - 'a')) + 0x20000000;

		switch (argv[1][1])
		{
			case 'a':
				base = GPIO_PORTA_BASE;
				break;

			case 'b':
				base = GPIO_PORTB_BASE;
				break;

			case 'c':
				base = GPIO_PORTC_BASE;
				break;

			case 'd':
				base = GPIO_PORTD_BASE;
				break;

			case 'e':
				base = GPIO_PORTE_BASE;
				break;

			case 'f':
				base = GPIO_PORTF_BASE;
				break;

			case 'g':
				base = GPIO_PORTG_BASE;
				break;

			case 'h':
				base = GPIO_PORTH_BASE;
				break;

			default:
				break;
		}
	}

	if (!(strcmp (argv[2], "in") == 0 || strcmp (argv[2], "out") == 0))
	{
		shell_printf ("MODE must be in/out!");
		shell_return ();
	}

	if (strcmp (argv[2], "in") == 0)
	{
		mode = 1;

		SysCtlPeripheralEnable(periph);
		GPIOPadConfigSet(base, pin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
		GPIODirModeSet(base, pin, GPIO_DIR_MODE_IN);
	}
	else 
	{
	 	mode = 0;

		SysCtlPeripheralEnable(periph);
		GPIOPadConfigSet(base, pin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD);
		GPIODirModeSet(base, pin, GPIO_DIR_MODE_OUT);
	}

	while (1)
	{
		if (mode == 1) //输入模式
		{
			state = GPIOPinRead(base, pin);

			if (state != backup_state)
			{
				times++;

				if (state == 0)			
				{
					shell_printf ("<- 0   [times = %d]\r\n", times);
				}
				else
				{
					shell_printf ("<- 1   [times = %d]\r\n", times); 	
				}

				backup_state = state;
			}
		}
		else		  //输出模式
		{
			shell_printf ("-> ");

			len = shell_read ((INT8U*)buf, sizeof (buf));
			if (len > 0)
			{
				if (len == 1 && buf[0] == '0')
				{
					GPIOPinWrite(base, pin, 0);
					shell_printf ("\r\n");
				}
				else if (len == 1 && buf[0] == '1')
				{
					GPIOPinWrite(base, pin, pin);
					shell_printf ("\r\n");
				}
				else
				{
				 	shell_printf ("\r\nerror!\r\n");
				}
			}
			else
			{
			 	shell_printf ("\r\n");
			}
		}
	}
}

void io_cmd_reg (void)
{
    shell_cmd_register ("io", io, "a io tool, it format is 'io PORT MODE'");
}

