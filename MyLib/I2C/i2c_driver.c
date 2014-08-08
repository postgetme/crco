#include "includes.h"

#define I2CWAITIME	5000000L
#define LASTBYTE	20000

/*******************************************************************************************
** 中断中的状态
*******************************************************************************************/ 
#define STATE_IDLE         0					/* 总线空闲状态 		*/
#define STATE_WRITE_ONE    1					/* 写单个字节状态 		*/
#define STATE_WRITE_NEXT   2					/* 写下一个字节状态 	*/
#define STATE_WRITE_FINAL_END  9				/* 写最后一个字节状态 	*/
#define STATE_WRITE_FINAL  3					/* 写最后一个字节状态 	*/
#define STATE_READ_ONE     4					/* 读单个字节状态 		*/
#define STATE_READ_FIRST   5					/* 读第一个字节状态 	*/
#define STATE_READ_NEXT    6					/* 读下一个字节状态 	*/
#define STATE_READ_FINAL   7					/* 读最后一个字节状态 	*/
#define STATE_READ_WAIT    8					/* 读字节等待状态 		*/

/*******************************************************************************************
** 全局变量定义
*******************************************************************************************/ 
static volatile INT8U 	I2C_sla0;				/* I2C器件从地址 					*/
static volatile INT32U	I2C_suba0;				/* I2C器件内部子地址 				*/
static volatile INT8U 	I2C_suba_num0;			/* I2C子地址字节数					*/
static volatile INT8U 	*I2C_buf0;        		/* 数据缓冲区指针  					*/
static volatile INT32U   I2C_num0;				/* 要读取/写入的数据个数 			*/
static volatile INT8U   I2C_opt0; 				/* I2c读写操作,0为读操作,1为写操作	*/
static volatile INT8U 	I2C_state0 = STATE_IDLE; /* 中断服务程序的当前状态  			*/

static volatile INT8U 	I2C_sla1;				/* I2C器件从地址 					*/
static volatile INT32U	I2C_suba1;				/* I2C器件内部子地址 				*/
static volatile INT8U 	I2C_suba_num1;			/* I2C子地址字节数					*/
static volatile INT8U 	*I2C_buf1;        		/* 数据缓冲区指针  					*/
static volatile INT32U   I2C_num1;				/* 要读取/写入的数据个数 			*/
static volatile INT8U   I2C_opt1; 				/* I2c读写操作,0为读操作,1为写操作	*/
static volatile INT8U 	I2C_state1 = STATE_IDLE; /* 中断服务程序的当前状态  			*/

//=0：正常；=1错误
static volatile int   	I2C0_ERR= 0; 			 /* I2c0读写出错	*/
static volatile int   	I2C1_ERR= 0; 			 /* I2c1读写出错	*/

//----------------------------------------------------------------------------------------
//函数声明
static void I2C0IntHandler (void);
static void I2C1IntHandler (void);

//x=100,延时100us，=2，延时2.5us，==500，延时250us
static void delay(int x)
{
	int i;
	while(x--)
	{
		i=10;
		while(i--) ;
	}
}

int I2CInit(INT8U id)
{
	switch (id)	{
		case 0:
	    	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C0);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOB);
	    	GPIOPinTypeI2C(GPIO_PORTB_BASE, GPIO_PIN_2 | GPIO_PIN_3);
			I2CIntRegister(I2C0_MASTER_BASE, I2C0IntHandler);
			//I2CMasterInitExpClk(I2C0_MASTER_BASE,SysCtlClockGet(),true);
			I2CMasterInitExpClk(I2C0_MASTER_BASE,SysCtlClockGet(),false);
	    	IntEnable(INT_I2C0);	
	    	I2CMasterIntEnable(I2C0_MASTER_BASE);
			break;

		case 1:
	    	SysCtlPeripheralEnable(SYSCTL_PERIPH_I2C1);
			SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	    	GPIOPinTypeI2C(GPIO_PORTA_BASE, GPIO_PIN_6 | GPIO_PIN_7);
			I2CIntRegister(I2C1_MASTER_BASE, I2C1IntHandler);
			//I2CMasterInitExpClk(I2C1_MASTER_BASE,SysCtlClockGet(),true);
			I2CMasterInitExpClk(I2C1_MASTER_BASE,SysCtlClockGet(),false);
	    	IntEnable(INT_I2C1);
	    	I2CMasterIntEnable(I2C1_MASTER_BASE);
			break;
			
		default:
			return (false);
	}
	return (true);
}

/*
*********************************************************************************************************
** 函数名称：ISendByte()
** 函数功能：向无子地址器件发送1字节数据。
** 入口参数：sla		器件地址
**           c			要发送的数据
** 出口参数：返回值为false时表示操作出错，为true时表示操作正确。
** 说明：	使用前要初始化好I2C引脚功能和I2C中断，并已使能I2C主模式
*********************************************************************************************************
*/ 
int ISendByte(INT8U id,INT8U sla, INT8U c)
{
	int cnt= 0;

	switch (id)	{
		case 0:
			I2C_sla0 = sla >> 1;
			I2C_buf0 = &c;
			I2C_state0 = STATE_WRITE_ONE;
			
			I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, I2C_sla0, false);			// 主机写操作
			I2CMasterDataPut(I2C0_MASTER_BASE, *I2C_buf0);					// 写一个数据
		    I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);	// 启动单次发送
			
			while(I2C_state0 != STATE_IDLE){ 
			    if(cnt++> I2CWAITIME){
					return (false);
				} 
		    }
			if(true == I2CMasterBusy(I2C0_MASTER_BASE))
				return (false);
			else
				return (true);

		case 1:
			I2C_sla1 = sla >> 1;
			I2C_buf1 = &c;
			I2C_state1 = STATE_WRITE_ONE;
			
			I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C_sla1, false);			// 主机写操作
			I2CMasterDataPut(I2C1_MASTER_BASE, *I2C_buf1);					// 写一个数据
		    I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);	// 启动单次发送
			
			while(I2C_state1 != STATE_IDLE){ 
			    if(cnt++ > I2CWAITIME){
					return (false);
				} 
		    }
			if(true == I2CMasterBusy(I2C1_MASTER_BASE))
				return (false);
			else
				return (true);

		default:
			return (false);
		}
}

/*
*********************************************************************************************************
** 函数名称 ：ISendStr()
** 函数功能 ：向有子地址器件任意地址开始写入N字节数据
** 入口参数 ：	sla			器件从地址
**				suba_type	子地址结构	0－8+X结构  1－单字节地址  2－双字节地址  3－三字节地址
**				suba		器件子地址
**				s			数据发送缓冲区指针
**				no			写入的个数
** 出口参数 ：	TRUE		操作成功
**				FALSE		操作失败
*********************************************************************************************************
*/ 
int ISendStr(INT8U id,INT8U sla, INT32U suba, INT8U sub_type, INT8U *s, INT32U no)
{
	int cnt= 0;

	switch (id)	{
		case 0:
			I2C_sla0 = sla >> 1;
			I2C_buf0 = s;
			I2C_num0 = no;
			I2C_suba0 = suba;
			I2C_opt0 = 0;
			I2C0_ERR= 0;	
			switch(sub_type)
			{
				// 子地址为“8+x”类型
				case 0:
					I2C_sla0 = I2C_sla0 | ((suba >> 8) & 0x07);
					I2C_suba_num0 = 1;
					break;	
				// 子地址为1个字节 	
				case 1:
					I2C_suba_num0 = 1;	
					break;
				// 子地址为2个字节
				case 2:
					I2C_suba_num0 = 2;	
					break;
				// 子地址为3个字节
				case 3:
					I2C_suba_num0 = 3;	
					break;
				default:
				    break;
			}
			
			if((no == 1) && (I2C_suba_num0 == 1))
				I2C_state0 = STATE_WRITE_FINAL;	// 单字节地址或8＋x结构地址发送1个字节数据
			else
				I2C_state0 = STATE_WRITE_NEXT;	// 多字节地址或发送多个字节数据	
								
			I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, I2C_sla0, false);		// 主机写操作										
		    I2C_suba_num0--;
		    I2CMasterDataPut(I2C0_MASTER_BASE, 
		    				(I2C_suba0 >> (8 * I2C_suba_num0)));			// 写子地址高字节
		    I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);// 突发写操作启动
		    
			while(I2C_state0 != STATE_IDLE){ 
			    if(cnt++> I2CWAITIME){
					return (false);
				} 
		    }

			delay(5000);

			cnt= 0;
			while(true == I2CMasterBusBusy(I2C0_MASTER_BASE) && (cnt < LASTBYTE))
			{
				cnt++;
			}
			
			if((cnt >= LASTBYTE) || (1 == I2C0_ERR))
			{
				return (false);
			}
			else
			{
				return (true);
			}

//			if(true == I2CMasterBusy(I2C0_MASTER_BASE))
//				return (false);
//			else
//				return (true);

		case 1:
			I2C_sla1 = sla >> 1;
			I2C_buf1 = s;
			I2C_num1 = no;
			I2C_suba1 = suba;
			I2C_opt1 = 0;
			I2C1_ERR= 0;	
			switch(sub_type)
			{
				// 子地址为“8+x”类型
				case 0:
					I2C_sla1 = I2C_sla1 | ((suba >> 8) & 0x07);
					I2C_suba_num1 = 1;
					break;	
				// 子地址为1个字节 	
				case 1:
					I2C_suba_num1 = 1;	
					break;
				// 子地址为2个字节
				case 2:
					I2C_suba_num1 = 2;	
					break;
				// 子地址为3个字节
				case 3:
					I2C_suba_num1 = 3;	
					break;
				default:
					break;
			}
			
			if((no == 1) && (I2C_suba_num1 == 1))
				I2C_state1 = STATE_WRITE_FINAL;										// 单字节地址或8＋x结构地址发送1个字节数据
			else
				I2C_state1 = STATE_WRITE_NEXT;										// 多字节地址或发送多个字节数据	
								
			I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C_sla1, false);				// 主机写操作										
		    I2C_suba_num1--;
		    I2CMasterDataPut(I2C1_MASTER_BASE, 
		    				(I2C_suba1 >> (8 * I2C_suba_num1)));					// 写子地址高字节
		    I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);	// 突发写操作启动
		    
			while(I2C_state1 != STATE_IDLE){ 
			    if(cnt++> I2CWAITIME){
					return (false);
				} 
		    }										//是否需要采用信号量等待方式？？？节省cpu资源									

			delay(5000);

			cnt= 0;
			while(true == I2CMasterBusBusy(I2C1_MASTER_BASE) && (cnt < LASTBYTE))
			{
				cnt++;
			}
			
			if((cnt >= LASTBYTE) || (1 == I2C1_ERR))
			{
				return (false);
			}
			else
			{
				return (true);
			}

// 			if(true == I2CMasterBusy(I2C1_MASTER_BASE))
//				return (false);
//			else
//				return (true);
		default:
			return (false);
		}
}

/*
*********************************************************************************************************
** 函数名称：ISendStrExt()
** 函数功能：向无子地址器件发送多字节数据。
** 入口参数：sla		器件地址
**           s			要发送的数据
**  		 no			发送的字节个数
** 出口参数：返回值为false时表示操作出错，为true时表示操作正确。
** 说明：	 发送数据字节至少2个字节
*********************************************************************************************************
*/ 
int ISendStrExt(INT8U id,INT8U sla, INT8U *s, INT8U no)
{
	int cnt= 0;

	switch (id)	{
		case 0:
			I2C_sla0 = sla >> 1;
			I2C_buf0 = s;
			I2C_num0 = no;
			if (no > 1)								// 发送的字节参数过滤
			{
				if(no == 2)
					I2C_state0 = STATE_WRITE_FINAL;
				else
					I2C_state0 = STATE_WRITE_NEXT;
			
				I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, I2C_sla0, false);	// 主机写操作
		    	I2CMasterDataPut(I2C0_MASTER_BASE, *I2C_buf0++);
		    	I2C_num0--;
		    	I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);// 突发写操作启动
		        
				while(I2C_state0 != STATE_IDLE);

				if(true == I2CMasterBusy(I2C0_MASTER_BASE))
					return (false);
				else
					return (true);
			}
			else 
				return(false);

		case 1:
			I2C_sla1 = sla >> 1;
			I2C_buf1 = s;
			I2C_num1 = no;
			if (no > 1)								// 发送的字节参数过滤
			{
				if(no == 2)
					I2C_state1 = STATE_WRITE_FINAL;
				else
					I2C_state1 = STATE_WRITE_NEXT;
			
				I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C_sla1, false);	// 主机写操作
		    	I2CMasterDataPut(I2C1_MASTER_BASE, *I2C_buf1++);
		    	I2C_num1--;
		    	I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);// 突发写操作启动
		        
				while(I2C_state1 != STATE_IDLE){ 
				    if(cnt++> I2CWAITIME){
						return (false);
					} 
			    }
				if(true == I2CMasterBusy(I2C1_MASTER_BASE))
					return (false);
				else
					return (true);
			}
			else 
				return (false);

		default:
			return (false);
		}
}

/*
*********************************************************************************************************
** 函数名称：IRcvByte()
** 函数功能：向无子地址器件读取1字节数据。
** 入口参数：sla		器件地址
**           c			接收数据的变量指针
** 出口参数：返回值为false时表示操作出错，为true时表示操作正确。
** 说明：	使用前要初始化好I2C引脚功能和I2C中断，并已使能I2C主模式
*********************************************************************************************************
*/
int IRcvByte(INT8U id,INT8U sla, INT8U *c)
{
	int cnt= 0;

	switch (id)	{
		case 0:
			I2C_sla0 = sla >> 1;
			I2C_buf0 = c;
			I2C_state0 = STATE_READ_WAIT;
			
			I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, I2C_sla0, true);	// 主机读操作
			I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);// 启动单次读取
			
			while(I2C_state0 != STATE_IDLE){ 
			    if(cnt++> I2CWAITIME){
					return (false);
				} 
		    }
			if(true == I2CMasterBusy(I2C0_MASTER_BASE))
				return (false);
			else
				return (true);

		case 1:
			I2C_sla1 = sla >> 1;
			I2C_buf1 = c;
			I2C_state1 = STATE_READ_WAIT;
			
			I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C_sla1, true);	// 主机读操作
			I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);// 启动单次读取
			
			while(I2C_state1 != STATE_IDLE){ 
			    if(cnt++> I2CWAITIME){
					return (false);
				} 
		    }
			if(true == I2CMasterBusy(I2C1_MASTER_BASE))
				return (false);
			else
				return (true);

		default:
			return (false);
		}

}

/*
*********************************************************************************************************
** 函数名称 ：IRcvStr()
** 函数功能 ：向有子地址器件读取N字节数据
** 入口参数 ：	sla			器件从地址
**				suba_type	子地址结构	0－8+X结构  1－单字节地址  2－双字节地址  3－三字节地址
**			  	suba		器件内部物理地址
**			  	*s			将要读取的数据的指针
**			  	no			将要读取的数据的个数
** 出口参数 ：	TRUE		操作成功
**			  	FALSE		操作失败
*********************************************************************************************************
*/
int IRcvStr(INT8U id,INT8U sla, INT32U suba, INT8U sub_type, INT8U *s, INT8U no)
{
	int cnt= 0;

	switch (id)	{
		case 0:
			I2C_sla0 = sla >> 1;
			I2C_buf0 = s;
			I2C_num0 = no;
			I2C_suba0 = suba;
			I2C_opt0 = 1;
			I2C0_ERR= 0;
						
			switch(sub_type)
			{
				// 子地址为“8+x”类型
				case 0:
					I2C_sla0 = I2C_sla0 | ((suba >> 8) & 0x07);
					I2C_suba_num0 = 1;
					break;	
				// 子地址为1个字节 	
				case 1:
					I2C_suba_num0 = 1;	
					break;
				// 子地址为2个字节
				case 2:
					I2C_suba_num0 = 2;	
					break;
				// 子地址为3个字节
				case 3:
					I2C_suba_num0 = 3;	
					break;
				default:
				    break;
			}
			
			if(I2C_suba_num0 == 1)
			{
				if(I2C_num0 == 1)
					I2C_state0 = STATE_READ_ONE;
				else 
				 	I2C_state0 = STATE_READ_FIRST;
			}
			else
				I2C_state0 = STATE_WRITE_NEXT;
								
			I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, I2C_sla0, false);		// 主机写操作										
		    I2C_suba_num0--;
		    I2CMasterDataPut(I2C0_MASTER_BASE, 
		    				(I2C_suba0 >> (8 * I2C_suba_num0)));			// 写子地址高字节
		    I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);// 突发写操作启动
		    
			while(I2C_state0 != STATE_IDLE){ 
			    if(cnt++> I2CWAITIME){
					return (false);
				} 
		    }

//			delay(2);

			cnt= 0;
			while(true == I2CMasterBusBusy(I2C0_MASTER_BASE) && (cnt < LASTBYTE))
			{
				cnt++;
			}
			
			if((cnt >= LASTBYTE) || (1 == I2C0_ERR))
			{
				return (false);
			}
			else
			{
				return (true);
			}

//			if(true == I2CMasterBusy(I2C0_MASTER_BASE))
//				return (false);
//			else
//				return (true);

		case 1:
			I2C_sla1 = sla >> 1;
			I2C_buf1 = s;
			I2C_num1 = no;
			I2C_suba1 = suba;
			I2C_opt1 = 1;
			I2C1_ERR= 0;

			switch(sub_type)
			{
				// 子地址为“8+x”类型
				case 0:
					I2C_sla1 = I2C_sla1 | ((suba >> 8) & 0x07);
					I2C_suba_num1 = 1;
					break;	
				// 子地址为1个字节 	
				case 1:
					I2C_suba_num1 = 1;	
					break;
				// 子地址为2个字节
				case 2:
					I2C_suba_num1 = 2;	
					break;
				// 子地址为3个字节
				case 3:
					I2C_suba_num1 = 3;	
					break;
				default:
					break;
			}
			
			if(I2C_suba_num1 == 1)
			{
				if(I2C_num1 == 1)
					I2C_state1 = STATE_READ_ONE;
				else 
				 	I2C_state1 = STATE_READ_FIRST;
			}
			else
				I2C_state1 = STATE_WRITE_NEXT;
								
			I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C_sla1, false);		// 主机写操作										
		    I2C_suba_num1--;
		    I2CMasterDataPut(I2C1_MASTER_BASE, 
		    				(I2C_suba1 >> (8 * I2C_suba_num1)));			// 写子地址高字节
		    I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);// 突发写操作启动
		    
			while(I2C_state1 != STATE_IDLE){ 
			    if(cnt++> I2CWAITIME){
					return (false);
				} 
		    }

//			delay(2);

			cnt= 0;
			while(true == I2CMasterBusBusy(I2C1_MASTER_BASE) && (cnt < LASTBYTE))
			{
				cnt++;
			}
			
			if((cnt >= LASTBYTE) || (1 == I2C1_ERR))
			{
				return (false);
			}
			else
			{
				return (true);
			}
		default:
			return (false);
		}
}

/*
*********************************************************************************************************
** 函数名称：IRvcStrExt()
** 函数功能：向无子地址器件读取N字节数据。
** 入口参数：sla		器件地址
**           s			接收数据的变量指针
**			 no			将要读取的数据的个数
** 出口参数：使用前要初始化好I2C引脚功能和I2C中断，并已使能I2C主模式
** 说明：	至少要读取2个字节
*********************************************************************************************************
*/
int IRvcStrExt(INT8U id,INT8U sla, INT8U *s, INT8U no)
{
	int cnt= 0;

	switch (id)	{
		case 0:
			I2C_sla0 = sla >> 1;
			I2C_buf0 = s;
			I2C_num0 = no;
			
			if(I2C_num0 > 1)
			{
				if(I2C_num0 == 2)
					I2C_state0 = STATE_READ_FINAL;
				else
					I2C_state0 = STATE_READ_NEXT;
					
				I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, I2C_sla0, true);	// 主机读操作
		    	I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);// 突发读操作启动
			
				while(I2C_state0 != STATE_IDLE){ 
				    if(cnt++> I2CWAITIME){
						return (false);
					} 
			    }
				if(true == I2CMasterBusy(I2C0_MASTER_BASE))
					return (false);
				else
					return (true);
			}
			else
				return (false);

		case 1:
			I2C_sla1 = sla >> 1;
			I2C_buf1 = s;
			I2C_num1 = no;
			
			if(I2C_num1 > 1)
			{
				if(I2C_num1 == 2)
					I2C_state1 = STATE_READ_FINAL;
				else
					I2C_state1 = STATE_READ_NEXT;
					
				I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C_sla1, true);	// 主机读操作
		    	I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);// 突发读操作启动
			
				while(I2C_state1 != STATE_IDLE){ 
				    if(cnt++> I2CWAITIME){
						return (false);
					} 
			    }
				if(true == I2CMasterBusy(I2C1_MASTER_BASE))
					return (false);
				else
					return (true);
			}
			else
				return (false);

		default:
			return (false);
		}
}

/*******************************************************************************************
** 函数名称 ：I2C_ISR()
** 函数功能 ：中断读写数据
** 入口参数 ：无
** 出口参数 ：无
*******************************************************************************************/
void I2C0IntHandler (void)
{
    I2CMasterIntClear(I2C0_MASTER_BASE);		// 清除I2C中断标志

	if(I2C_MASTER_ERR_NONE == I2CMasterErr(I2C0_MASTER_BASE))
	{
		;	
	}
	else
	{
		I2C0_ERR= 1;
    	I2C_state0 = STATE_IDLE;		
	}

    switch(I2C_state0)
    {
        // 空闲状态
        case STATE_IDLE:
        {
            break;
        }
        // 写完最后一个字节状态
        case STATE_WRITE_FINAL_END:
        {
        	I2C_state0 = STATE_IDLE;
        	break;
        }

        // 写完单个字节状态
        case STATE_WRITE_ONE:
        {
        	I2C_state0 = STATE_IDLE;
        	break;
        }
		// 写下一个数据
        case STATE_WRITE_NEXT:
        {
        	// 将下一个字节写入寄存器
            if(I2C_suba_num0 != 0)
            {
            	I2C_suba_num0--;
            	I2CMasterDataPut(I2C0_MASTER_BASE, 
            					(I2C_suba0 >> (8 * I2C_suba_num0)));
            	if((I2C_suba_num0 == 0) && I2C_opt0 == 1)
            	{
            		if(I2C_num0 == 1)
            			I2C_state0 = STATE_READ_ONE;	
            		else
            			I2C_state0 = STATE_READ_FIRST;	
            	}

            	if((I2C_suba_num0 == 0) && I2C_opt0 == 0)
            	{
            		if(I2C_num0 == 1)
            			I2C_state0 = STATE_WRITE_FINAL;	
            		else
            			I2C_state0 = STATE_WRITE_NEXT;	
            	}
            	
            }            
			else
            {
             	I2CMasterDataPut(I2C0_MASTER_BASE, *I2C_buf0++);
            	I2C_num0--;             
            	if(I2C_num0 == 1)
            	{
                	I2C_state0 = STATE_WRITE_FINAL;
            	}
//            	I2C_num0--;				          
            }
            // 继续执行块写操作(run=1)
            I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
            break;
        }
        // 写最后一个数据
        case STATE_WRITE_FINAL:
        {
            I2CMasterDataPut(I2C0_MASTER_BASE, *I2C_buf0);
            I2C_num0--;
            // 发送停止
            I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
			// 下一个状态为块写完成
            I2C_state0= STATE_IDLE;
            break;
        }
 
 		// 读取一个字节
        case STATE_READ_ONE:
        {

            I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, I2C_sla0, true);

            I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

            I2C_state0= STATE_READ_WAIT;

            break;
        }
		// 读取多个字节开始
        case STATE_READ_FIRST:
        {

            I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, I2C_sla0, true);

            I2CMasterControl(I2C0_MASTER_BASE,
                             I2C_MASTER_CMD_BURST_RECEIVE_START);

            if(I2C_num0 == 2)
                I2C_state0 = STATE_READ_FINAL;
            else
            	I2C_state0 = STATE_READ_NEXT;

            break;
        }
		// 读取下一个字节
        case STATE_READ_NEXT:
        {

            *I2C_buf0++ = I2CMasterDataGet(I2C0_MASTER_BASE);
            I2C_num0--;

            I2CMasterControl(I2C0_MASTER_BASE,
                             I2C_MASTER_CMD_BURST_RECEIVE_CONT);

            if(I2C_num0 == 2)
                I2C_state0 = STATE_READ_FINAL;

            break;
        }
		// 读取最后一个字节
        case STATE_READ_FINAL:
        {

            *I2C_buf0++ = I2CMasterDataGet(I2C0_MASTER_BASE);
            I2C_num0--;

            I2CMasterControl(I2C0_MASTER_BASE,
                             I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

            I2C_state0= STATE_READ_WAIT;
            break;
        }
		// 等待读取一个字节
        case STATE_READ_WAIT:
        {
 
            *I2C_buf0++  = I2CMasterDataGet(I2C0_MASTER_BASE); // 读取数据
            I2C_num0--;

            I2C_state0= STATE_IDLE;
            break;
        }
    }
}
/*******************************************************************************************
** 函数名称 ：I2C_ISR()
** 函数功能 ：中断读写数据
** 入口参数 ：无
** 出口参数 ：无
*******************************************************************************************/
void I2C1IntHandler (void)
{
    I2CMasterIntClear(I2C1_MASTER_BASE);		// 清除I2C中断标志

	if(I2C_MASTER_ERR_NONE == I2CMasterErr(I2C1_MASTER_BASE))
	{
		;	
	}
	else
	{
		I2C1_ERR= 1;
    	I2C_state1 = STATE_IDLE;		
	}

    switch(I2C_state1)
    {
        // 空闲状态
        case STATE_IDLE:
        {
            break;
        }
        // 写完最后一个字节状态
        case STATE_WRITE_FINAL_END:
        {
        	I2C_state1 = STATE_IDLE;
        	break;
        }
        // 写完单个字节状态
        case STATE_WRITE_ONE:
        {
        	I2C_state1 = STATE_IDLE;
        	break;
        }
		// 写下一个数据
        case STATE_WRITE_NEXT:
        {
        	// 将下一个字节写入寄存器
            if(I2C_suba_num1 != 0)
            {
            	I2C_suba_num1--;
            	I2CMasterDataPut(I2C1_MASTER_BASE, 
            					(I2C_suba1 >> (8 * I2C_suba_num1)));
            	if((I2C_suba_num1 == 0) && I2C_opt1 == 1)
            	{
            		if(I2C_num1 == 1)
            			I2C_state1 = STATE_READ_ONE;	
            		else
            			I2C_state1 = STATE_READ_FIRST;	
            	}

            	if((I2C_suba_num1 == 0) && I2C_opt1 == 0)
            	{
            		if(I2C_num1 == 1)
            			I2C_state1 = STATE_WRITE_FINAL;	
            		else
            			I2C_state1 = STATE_WRITE_NEXT;	
            	}
            	
            }            
			else
            {
             	I2CMasterDataPut(I2C1_MASTER_BASE, *I2C_buf1++);
            	I2C_num1--;             
            	if(I2C_num1 == 1)
            	{
                	I2C_state1 = STATE_WRITE_FINAL;
            	}
//            	I2C_num1--;				          
            }
            // 继续执行块写操作(run=1)
            I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
            break;
        }
        // 写最后一个数据
        case STATE_WRITE_FINAL:
        {
            I2CMasterDataPut(I2C1_MASTER_BASE, *I2C_buf1);
            I2C_num1--;
            // 发送停止
            I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
			// 下一个状态为块写完成
            I2C_state1= STATE_WRITE_FINAL_END;
            break;
        }
 
 		// 读取一个字节
        case STATE_READ_ONE:
        {

            I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C_sla1, true);

            I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

            I2C_state1= STATE_READ_WAIT;

            break;
        }
		// 读取多个字节开始
        case STATE_READ_FIRST:
        {

            I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C_sla1, true);

            I2CMasterControl(I2C1_MASTER_BASE,
                             I2C_MASTER_CMD_BURST_RECEIVE_START);

            if(I2C_num1 == 2)
                I2C_state1 = STATE_READ_FINAL;
            else
            	I2C_state1 = STATE_READ_NEXT;

            break;
        }
		// 读取下一个字节
        case STATE_READ_NEXT:
        {

            *I2C_buf1++ = I2CMasterDataGet(I2C1_MASTER_BASE);
            I2C_num1--;

            I2CMasterControl(I2C1_MASTER_BASE,
                             I2C_MASTER_CMD_BURST_RECEIVE_CONT);

            if(I2C_num1 == 2)
                I2C_state1 = STATE_READ_FINAL;

            break;
        }
		// 读取最后一个字节
        case STATE_READ_FINAL:
        {
            *I2C_buf1++ = I2CMasterDataGet(I2C1_MASTER_BASE);
            I2C_num1--;

            I2CMasterControl(I2C1_MASTER_BASE,
                             I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

            I2C_state1= STATE_READ_WAIT;
            break;
        }
		// 等待读取一个字节
        case STATE_READ_WAIT:
        {
 
            *I2C_buf1++  = I2CMasterDataGet(I2C1_MASTER_BASE); // 读取数据
            I2C_num1--;

            I2C_state1= STATE_IDLE;
            break;
        }
    }
}
