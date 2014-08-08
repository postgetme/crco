#ifndef UART_H_H
#define	UART_H_H

#define UART_UNPEND_RECV        0      // 串口非阻塞方式接收
#define UART_PEND_RECV          1      // 串口阻塞方式接收
#define UART_GET_LINK_STATE		2	   // 获取通道的连接状态
#define UART_FRAME_TIMEOUT		3      // 设置字节流超时时间
#define UART_FORCE_ODD			4	   // 强制操作ODD位

#define P_NONE					0
#define P_ODD					1
#define P_EVEN					2
#define P_ONE					3
#define P_ZERO					4

#define S_ONE					1	
#define S_TWO					2

extern INT8S uart_init (INT8U id, INT32U baud_rate, INT8U data_bits, INT8U parity, INT8U stop_bits, char* pin_485);
extern INT16U uart_write (INT8U id, INT8U* pdat, INT16U len);
extern INT16U uart_read (INT8U id, INT8U* pdat, INT16U len);
extern INT8S uart_ioctl (INT8U id, INT8U cmd, INT32U arg);

#endif
