#include "includes.h"

#define UART_RECV_BUF_SIZE 1024

typedef struct
{
	INT8U* pdat;
	INT16U size;
	INT16U front;
	INT16U rear;
	INT16U count;
}QUEUE;

static INT8S queue_create (QUEUE *q, INT8U* pdat, INT16U len)
{
    q->pdat = pdat;
    q->size = len;
    q->front = 0;
    q->rear = 0;
    q->count = 0;

    return 0;
}

static INT8S queue_in (QUEUE* q, INT8U dat)
{
    OS_CPU_SR  cpu_sr;

	OS_ENTER_CRITICAL();

	if (q->front == q->rear && q->count == q->size)
	{
		OS_EXIT_CRITICAL();
	    return -1;
	}
	else
	{
		q->pdat[q->rear] = dat;
		q->rear = (q->rear + 1) % q->size;
		q->count = q->count + 1;
		OS_EXIT_CRITICAL();
		return 0;
	}
}

static INT8S queue_out (QUEUE* q,INT8U* pdat)
{
    OS_CPU_SR  cpu_sr;

	OS_ENTER_CRITICAL();

	if((q->front == q->rear) && (q->count == 0))
	{
		OS_EXIT_CRITICAL();
		return -1;
	}
	else
	{
		*pdat = q->pdat[q->front];
		q->front = (q->front + 1) % q->size;
		q->count = q->count - 1;
		OS_EXIT_CRITICAL();
		return 0;
	}
}

typedef struct
{
	INT32U cfg;
	INT32U baud_rate;
	INT16U recv_timeout;
	INT8U  frame_timeout;
	char pin_485[10];
	OS_EVENT* recv_sem;
	QUEUE recv_q;
	INT8U recv_q_buf[UART_RECV_BUF_SIZE];
}UART_ATTR;

UART_ATTR uart_attr[3];

static INT32U get_uart_base (INT8U id)
{
	if (id == 0)
	{
		return 	UART0_BASE;
	}
	else if (id == 1)
	{
		return 	UART1_BASE;
	}
	else if (id == 2)
	{
		return 	UART2_BASE;
	}
	else
	{
	    return UART0_BASE;
	}
}

static void uart0_handle (void)
{
    INT32U status;
	INT32S dat;

    status = UARTIntStatus(UART0_BASE, true);
	UARTIntClear(UART0_BASE, status);

	while (1)
	{
		dat = UARTCharGetNonBlocking(UART0_BASE);
	 	if (dat == -1)
	 	{
			// FIFO已空
	   		break;
		}
		// 将读到的数据存入数据缓冲区
		queue_in (&uart_attr[0].recv_q, dat);
	}

	// 发送信号量通知接收任务接收立即接收任务。
	OSSemPost(uart_attr[0].recv_sem);
}

static void uart1_handle (void)
{
    INT32U status;
	INT32S dat;

    status = UARTIntStatus(UART1_BASE, true);
	UARTIntClear(UART1_BASE, status);

	while (1)
	{
		dat = UARTCharGetNonBlocking(UART1_BASE);
	 	if (dat == -1)
	 	{
	   		break;
		}
		queue_in (&uart_attr[1].recv_q, dat);
	}

	OSSemPost(uart_attr[1].recv_sem);
}

static void uart2_handle (void)
{
    INT32U status;
	INT32S dat;

    status = UARTIntStatus(UART2_BASE, true);
	UARTIntClear(UART2_BASE, status);

	while (1)
	{
		dat = UARTCharGetNonBlocking(UART2_BASE);
	 	if (dat == -1)
	 	{
	   		break;
		}
		queue_in (&uart_attr[2].recv_q, dat);
	}

	OSSemPost(uart_attr[2].recv_sem);
}

INT8S uart_init (INT8U id, INT32U baud_rate, INT8U data_bits, INT8U parity, INT8U stop_bits, char* pin_485)
{
	uart_attr[id].baud_rate = baud_rate;

	switch (baud_rate) {
    	case 2400:
    	case 4800:
    	case 9600:
    		uart_attr[id].frame_timeout = 3;
    		break;
    	case 38400:
    	case 19200:
    		uart_attr[id].frame_timeout = 2;
    		break;
    	case 57600:
    	case 115200:
    		uart_attr[id].frame_timeout = 1;
    		break;
    	default:
    	    uart_attr[id].frame_timeout = 3;
    		return -1;
	}

	switch (data_bits) {
		case 5:
			uart_attr[id].cfg = UART_CONFIG_WLEN_5;
			break;
		case 6:
			uart_attr[id].cfg = UART_CONFIG_WLEN_6;
			break;
		case 7:
			uart_attr[id].cfg = UART_CONFIG_WLEN_7;
			break;
		case 8:
			uart_attr[id].cfg = UART_CONFIG_WLEN_8;
			break;
    	default:
    		return -1;
	}

	switch (parity) {
		case P_NONE:
			uart_attr[id].cfg |= UART_CONFIG_PAR_NONE;
			break;
		case P_ODD:
			uart_attr[id].cfg |= UART_CONFIG_PAR_ODD;
			break;
		case P_EVEN:
			uart_attr[id].cfg |= UART_CONFIG_PAR_EVEN;
			break;
		case P_ONE:
			uart_attr[id].cfg |= UART_CONFIG_PAR_ONE;
			break;
		case P_ZERO:
			uart_attr[id].cfg |= UART_CONFIG_PAR_ZERO;
			break;
    	default:
    		return -1;
	}

	switch (stop_bits) {
		case S_ONE:
			uart_attr[id].cfg |= UART_CONFIG_STOP_ONE;
			break;
		case S_TWO:
			uart_attr[id].cfg |= UART_CONFIG_STOP_TWO;
			break;
    	default:
    		return -1;
	}

	switch (id)	{
		case 0:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    		SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
    		GPIOPinTypeUART(GPIO_PORTA_BASE , GPIO_PIN_0 | GPIO_PIN_1);
			UARTIntRegister(UART0_BASE, uart0_handle);
			uart_attr[0].recv_sem = OSSemCreate(0);
			queue_create(&uart_attr[0].recv_q, uart_attr[0].recv_q_buf, UART_RECV_BUF_SIZE);
			break;
		case 1:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_UART1);
			GPIOPinTypeUART(GPIO_PORTD_BASE, GPIO_PIN_2|GPIO_PIN_3);
			UARTIntRegister(UART1_BASE, uart1_handle);
			uart_attr[1].recv_sem = OSSemCreate(0);
			queue_create(&uart_attr[1].recv_q, uart_attr[1].recv_q_buf, UART_RECV_BUF_SIZE);
			break;
		case 2:
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOG);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_UART2);
			GPIOPinTypeUART(GPIO_PORTG_BASE, GPIO_PIN_0|GPIO_PIN_1);
			UARTIntRegister(UART2_BASE, uart2_handle);
			uart_attr[2].recv_sem = OSSemCreate(0);
			queue_create(&uart_attr[2].recv_q, uart_attr[2].recv_q_buf, UART_RECV_BUF_SIZE);
			break;
		default:
			return -1;
	}

	UARTIntDisable(get_uart_base (id), UART_INT_TX|UART_INT_RX | UART_INT_RT);
    UARTConfigSet(get_uart_base (id), uart_attr[id].baud_rate, uart_attr[id].cfg);
    UARTEnable(get_uart_base (id));
    UARTIntEnable(get_uart_base (id), UART_INT_RX | UART_INT_RT);

	uart_attr[id].recv_timeout = 0xffff;

	memset (uart_attr[id].pin_485, 0, 10);

	if (pin_485 != NULL)
	{
        strcpy (uart_attr[id].pin_485, pin_485);
	}

	return 0;
}

INT16U uart_write (INT8U id, INT8U* buf, INT16U len)
{
    INT32U uart_base;

	//r_led_flash ();

    uart_base = get_uart_base (id);

	UARTIntDisable(uart_base, UART_INT_TX | UART_INT_RX | UART_INT_RT);

	if (uart_attr[id].pin_485 != NULL)
	{
	    io_write (uart_attr[id].pin_485, 1);
	}

	while(len--)
    {
        UARTCharPut(uart_base, *buf++);
    }

	if (uart_attr[id].pin_485 != NULL)
	{
	    io_write (uart_attr[id].pin_485, 0);
	}

	UARTIntEnable(uart_base, UART_INT_RX | UART_INT_RT);

	return len;
}

INT16U uart_read (INT8U id, INT8U* buf, INT16U len)
{
	INT8U err;
	INT16U cnt = 0;

	// 等待从串口接收数据
	OSSemPend(uart_attr[id].recv_sem, uart_attr[id].recv_timeout, &err);
	if (err == OS_TIMEOUT) {
		return 0;
	}
	// 读空串口FIFO接收缓冲区
	while (1) {
	 	if (queue_out(&uart_attr[id].recv_q, buf) == -1) {
			// FIFO已空
		   	break;
		}
		// 将读到的数据存入数据缓冲区
		buf++;
		cnt++;
	}
	do {// 等待从串口接收数据
		OSSemPend(uart_attr[id].recv_sem, uart_attr[id].frame_timeout, &err);
		if (err == OS_TIMEOUT) {
			if (cnt >= len)
			{
			 	return len;
			}
			else
			{
				return cnt;
			}
		}
		// 读空串口FIFO接收缓冲区
		while (1) {
			if (queue_out(&uart_attr[id].recv_q, buf) == -1) {
				// FIFO已空
		   		break;
			}
			// 将读到的数据存入数据缓冲区
			buf++;
			cnt ++;
			if (cnt >= len) {
				// 接收数据已超过有效的范围。
				return len;
			}
		}
	}while(1);
}

INT8S uart_ioctl (INT8U id, INT8U cmd, INT32U arg)
{
	INT8S err = 0;

	switch (cmd) {
		case UART_UNPEND_RECV:
			uart_attr[id].recv_timeout = arg;
			break;
		case UART_PEND_RECV:
			uart_attr[id].recv_timeout = 0xffff;
			break;
		case UART_GET_LINK_STATE:
			*(INT32U*)arg = 1;
			break;
		case UART_FRAME_TIMEOUT:
			uart_attr[id].frame_timeout = arg;
			break;
		case UART_FORCE_ODD:
			if (arg == 0) {
				// UART_CONFIG_PAR_ONE 为强制低电平
				uart_attr[id].cfg = uart_attr[id].cfg | UART_CONFIG_PAR_ONE;
			}else {
				// UART_CONFIG_PAR_ZERO 为强制高电平
				uart_attr[id].cfg = uart_attr[id].cfg | UART_CONFIG_PAR_ZERO;
				uart_attr[id].cfg = uart_attr[id].cfg & 0xFFFFFFFB;
			}
			HWREG(get_uart_base(id) + UART_O_LCRH) = uart_attr[id].cfg;
			break;
		default:
			err = FALSE;
			break;
	}
	return err;
}
