/*������Զ*/
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
      *pucData++ = UARTCharNonBlockingGet(ulBase);     /* ������յ������� */
    }
}

/*********************************************************************************************************
** ��������: RecFrame
** ��������: ����XMODEM��һ֡����
** ��ڲ���: oneFrame      ֡���ݽṹ�����ĵ�ַ
**           preblkNum     Ԥ��İ���
** ���ڲ���: 0             ���յ�֡������ȷ
**           1             ���յ�֡���ݴ���
*********************************************************************************************************/
static int RecFrame(struct Frame *oneFrame,unsigned char preblkNum)
{
	int returnVal;	

	oneFrame->blkNum = UARTCharGet(UART0_BASE);                           /* �ȴ����հ���                 */
	oneFrame->blkNumRev = UARTCharGet(UART0_BASE);                        /* �ȴ����հ��ŵĲ���           */
  
  	Uart0Receive(UART0_BASE,oneFrame->dataBlk,BLOCK_SIZE);                           /* �ȴ��������ݰ���128���ֽ�    */
		
	oneFrame->checkSum = UARTCharGet(UART0_BASE);                         /* �ȴ������ۼ�У���           */

	if( (oneFrame->blkNum != 255 - oneFrame->blkNumRev) &&                /* У����Ų���                 */
      (oneFrame->checkSum == CheckSum(oneFrame)) &&                     /* У���ۼ�У���               */
      (oneFrame->blkNum != preblkNum) ) {                               /* У��Ԥ����ź�ʵ�ʰ���       */
	  returnVal = 1;
  }	else {
	  returnVal = 0;
  }
	return (returnVal);	
}

/*********************************************************************************************************
** ��������: XmodemRun
** ��������: XmodemЭ�鴫�亯��
** ��ڲ���: ptr           Xmodem֡���ݽ��ջ�����
** ���ڲ���: true          ͨ�ųɹ�
**           false         ���ͷ���ֹͨ�� 
*********************************************************************************************************/
uint8 XmodemRun(char* file_name)
{	
  struct Frame dataFrame;
  uint8 RcvblkNum = 1;                                                  /* Ԥ��İ���                   */
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
    TimeDelay(ONE_SECOND/5);                                              /* 1S�ӵ�TimeOut                */
    Uart0PutC(NAK);                                                     /* ����Xmodem�Ĵ�����ʼ�ź�     */
    while(!UARTCharsAvail(UART0_BASE)) {                                /* �ж�Xmodem�Ƿ�������       */
      count++;
      TimeDelay(ONE_SECOND / 200000);
      if(count > 2000) {
        count = 0;
        break;
      }
    }
  } while(!UARTCharsAvail(UART0_BASE));
  
	do {
    switch(UARTCharGet(UART0_BASE)) {                                   /* ��ȡXmodem����ʼ�ֽ�         */
		  case SOH:                                                         /* ��ʼ�ֽ�ΪSOH����ʾ֡��ʼ    */
			  switch( RecFrame(&dataFrame,RcvblkNum) ) {                      /* ����֡����                   */
					case 0:                                                       /* ֡���ݽ�������               */
					  	fwrite (dataFrame.dataBlk, 1, 128, f); 

            			RcvblkNum++;                                                /* ���ŵ���                     */ 
						Uart0PutC(ACK);                                             /* ����ACK����Ӧ������ȷ        */
                                                                        /* ��׼��������һ֡             */
						break;
				  case 1:                                                       /* ֡���ݽ��մ���               */
						Uart0PutC(NAK);                                             /* ����NAK����֪���·��͸�֡    */
						break;	
		          default:
		            	break;
				}		
				break;			
			case EOT:                                                         /* ���յ��ֽ�EOT                */
                                                                        /* ����Xmodem���Ͷ��Ѿ�������� */
				Uart0PutC(ACK); 
				fclose (f);                                                /* ��ӦACK����˫���˿�����      */
        		return true;
          
      		case CAN:                                                         /* ���ͷ���ֹ����               */
       			return false;
					
				default:
					break;	
		}
  } while(1);
}

/*********************************************************************************************************
** ��������: CheckSum
** ��������: ���ۼ�У���
** ��ڲ���: oneFrame      Xmodem֡�ṹ����
** ���ڲ���: Xmodem���������ۼ�У���
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
