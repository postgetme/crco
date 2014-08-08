/*广州致远*/
#include "includes.h"
#include "xmodem.h"

#define  Uart0PutC(x)    (UARTCharPut(UART0_BASE , (x)))

static void  TimeDelay(unsigned long  ulVal)
{
    while ( --ulVal  !=  0 );
}

static void Uart0Receive(unsigned long ulBase, unsigned char *pucData, unsigned long ulSize)
{
    while(ulSize--) {
      while(!UARTCharsAvail(ulBase));
      *pucData++ = UARTCharNonBlockingGet(ulBase);     /* 保存接收到的数据 */
    }
}

/*********************************************************************************************************
** 函数名称: RecFrame
** 功能描述: 接受XMODEM的一帧数据
** 入口参数: oneFrame      帧数据结构变量的地址
**           preblkNum     预测的包号
** 出口参数: 0             接收的帧数据正确
**           1             接收的帧数据错误
*********************************************************************************************************/
static int RecFrame(struct Frame *oneFrame,unsigned char preblkNum)
{
	int returnVal;	

	oneFrame->blkNum = UARTCharGet(UART0_BASE);                           /* 等待接收包号                 */
	oneFrame->blkNumRev = UARTCharGet(UART0_BASE);                        /* 等待接收包号的补码           */
  
  	Uart0Receive(UART0_BASE,oneFrame->dataBlk,BLOCK_SIZE);                           /* 等待接收数据包的128个字节    */
		
	oneFrame->checkSum = UARTCharGet(UART0_BASE);                         /* 等待接收累加校验和           */

	if( (oneFrame->blkNum != 255 - oneFrame->blkNumRev) &&                /* 校验包号补码                 */
      (oneFrame->checkSum == CheckSum(oneFrame)) &&                     /* 校验累加校验和               */
      (oneFrame->blkNum != preblkNum) ) {                               /* 校验预测包号和实际包号       */
	  returnVal = 1;
  }	else {
	  returnVal = 0;
  }
	return (returnVal);	
}

/*********************************************************************************************************
** 函数名称: XmodemRun
** 功能描述: Xmodem协议传输函数
** 入口参数: ptr           Xmodem帧数据接收缓冲区
** 出口参数: true          通信成功
**           false         发送方中止通信 
*********************************************************************************************************/
uint8 XmodemRun(char* file_name)
{	
  struct Frame dataFrame;
  uint8 RcvblkNum = 1;                                                  /* 预测的包号                   */
  uint16 count = 0;
  FILE* f = NULL;

	f = fopen (file_name, "r");
	
	if (f == NULL)
	{
		f = fopen (file_name, "w");
	}
	else
	{
		printf ("'%s' already exist!", file_name);
		fclose (f);
		return false; 	
	}
  
  do {
    TimeDelay(ONE_SECOND/5);                                              /* 1S钟的TimeOut                */
    Uart0PutC(NAK);                                                     /* 发送Xmodem的传输起始信号     */
    while(!UARTCharsAvail(UART0_BASE)) {                                /* 判断Xmodem是否建立连接       */
      count++;
      TimeDelay(ONE_SECOND / 200000);
      if(count > 2000) {
        count = 0;
        break;
      }
    }
  } while(!UARTCharsAvail(UART0_BASE));
  
	do {
    switch(UARTCharGet(UART0_BASE)) {                                   /* 获取Xmodem的起始字节         */
		  case SOH:                                                         /* 起始字节为SOH，表示帧起始    */
			  switch( RecFrame(&dataFrame,RcvblkNum) ) {                      /* 接收帧数据                   */
					case 0:                                                       /* 帧数据接收正常               */
					  	fwrite (dataFrame.dataBlk, 1, 128, f); 

            			RcvblkNum++;                                                /* 包号递增                     */ 
						Uart0PutC(ACK);                                             /* 发送ACK，回应接收正确        */
                                                                        /* 并准备接收下一帧             */
						break;
				  case 1:                                                       /* 帧数据接收错误               */
						Uart0PutC(NAK);                                             /* 发送NAK，告知重新发送该帧    */
						break;	
		          default:
		            	break;
				}		
				break;			
			case EOT:                                                         /* 接收到字节EOT                */
                                                                        /* 表明Xmodem发送端已经发送完毕 */
				Uart0PutC(ACK); 
				fclose (f);                                                /* 回应ACK表明双方端开连接      */
        		return true;
          
      		case CAN:                                                         /* 发送方中止传输               */
       			return false;
					
				default:
					break;	
		}
  } while(1);
}

/*********************************************************************************************************
** 函数名称: CheckSum
** 功能描述: 求累加校验和
** 入口参数: oneFrame      Xmodem帧结构数据
** 出口参数: Xmodem数据区的累加校验和
*********************************************************************************************************/
static unsigned char CheckSum(struct Frame *oneFrame)
{
	int i;
	unsigned char dataSum = 0x00;
	
	for(i = 0; i < BLOCK_SIZE; ++i)	{
		dataSum += oneFrame->dataBlk[i];
	}
  return (dataSum & 0xff);
}
/*********************************************************************************************************
**                            End Of File
********************************************************************************************************/
