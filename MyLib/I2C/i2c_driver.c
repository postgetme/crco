#include "includes.h"

#define I2CWAITIME	5000000L
#define LASTBYTE	20000

/*******************************************************************************************
** �ж��е�״̬
*******************************************************************************************/ 
#define STATE_IDLE         0					/* ���߿���״̬ 		*/
#define STATE_WRITE_ONE    1					/* д�����ֽ�״̬ 		*/
#define STATE_WRITE_NEXT   2					/* д��һ���ֽ�״̬ 	*/
#define STATE_WRITE_FINAL_END  9				/* д���һ���ֽ�״̬ 	*/
#define STATE_WRITE_FINAL  3					/* д���һ���ֽ�״̬ 	*/
#define STATE_READ_ONE     4					/* �������ֽ�״̬ 		*/
#define STATE_READ_FIRST   5					/* ����һ���ֽ�״̬ 	*/
#define STATE_READ_NEXT    6					/* ����һ���ֽ�״̬ 	*/
#define STATE_READ_FINAL   7					/* �����һ���ֽ�״̬ 	*/
#define STATE_READ_WAIT    8					/* ���ֽڵȴ�״̬ 		*/

/*******************************************************************************************
** ȫ�ֱ�������
*******************************************************************************************/ 
static volatile INT8U 	I2C_sla0;				/* I2C�����ӵ�ַ 					*/
static volatile INT32U	I2C_suba0;				/* I2C�����ڲ��ӵ�ַ 				*/
static volatile INT8U 	I2C_suba_num0;			/* I2C�ӵ�ַ�ֽ���					*/
static volatile INT8U 	*I2C_buf0;        		/* ���ݻ�����ָ��  					*/
static volatile INT32U   I2C_num0;				/* Ҫ��ȡ/д������ݸ��� 			*/
static volatile INT8U   I2C_opt0; 				/* I2c��д����,0Ϊ������,1Ϊд����	*/
static volatile INT8U 	I2C_state0 = STATE_IDLE; /* �жϷ������ĵ�ǰ״̬  			*/

static volatile INT8U 	I2C_sla1;				/* I2C�����ӵ�ַ 					*/
static volatile INT32U	I2C_suba1;				/* I2C�����ڲ��ӵ�ַ 				*/
static volatile INT8U 	I2C_suba_num1;			/* I2C�ӵ�ַ�ֽ���					*/
static volatile INT8U 	*I2C_buf1;        		/* ���ݻ�����ָ��  					*/
static volatile INT32U   I2C_num1;				/* Ҫ��ȡ/д������ݸ��� 			*/
static volatile INT8U   I2C_opt1; 				/* I2c��д����,0Ϊ������,1Ϊд����	*/
static volatile INT8U 	I2C_state1 = STATE_IDLE; /* �жϷ������ĵ�ǰ״̬  			*/

//=0��������=1����
static volatile int   	I2C0_ERR= 0; 			 /* I2c0��д����	*/
static volatile int   	I2C1_ERR= 0; 			 /* I2c1��д����	*/

//----------------------------------------------------------------------------------------
//��������
static void I2C0IntHandler (void);
static void I2C1IntHandler (void);

//x=100,��ʱ100us��=2����ʱ2.5us��==500����ʱ250us
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
** �������ƣ�ISendByte()
** �������ܣ������ӵ�ַ��������1�ֽ����ݡ�
** ��ڲ�����sla		������ַ
**           c			Ҫ���͵�����
** ���ڲ���������ֵΪfalseʱ��ʾ��������Ϊtrueʱ��ʾ������ȷ��
** ˵����	ʹ��ǰҪ��ʼ����I2C���Ź��ܺ�I2C�жϣ�����ʹ��I2C��ģʽ
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
			
			I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, I2C_sla0, false);			// ����д����
			I2CMasterDataPut(I2C0_MASTER_BASE, *I2C_buf0);					// дһ������
		    I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);	// �������η���
			
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
			
			I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C_sla1, false);			// ����д����
			I2CMasterDataPut(I2C1_MASTER_BASE, *I2C_buf1);					// дһ������
		    I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_SINGLE_SEND);	// �������η���
			
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
** �������� ��ISendStr()
** �������� �������ӵ�ַ���������ַ��ʼд��N�ֽ�����
** ��ڲ��� ��	sla			�����ӵ�ַ
**				suba_type	�ӵ�ַ�ṹ	0��8+X�ṹ  1�����ֽڵ�ַ  2��˫�ֽڵ�ַ  3�����ֽڵ�ַ
**				suba		�����ӵ�ַ
**				s			���ݷ��ͻ�����ָ��
**				no			д��ĸ���
** ���ڲ��� ��	TRUE		�����ɹ�
**				FALSE		����ʧ��
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
				// �ӵ�ַΪ��8+x������
				case 0:
					I2C_sla0 = I2C_sla0 | ((suba >> 8) & 0x07);
					I2C_suba_num0 = 1;
					break;	
				// �ӵ�ַΪ1���ֽ� 	
				case 1:
					I2C_suba_num0 = 1;	
					break;
				// �ӵ�ַΪ2���ֽ�
				case 2:
					I2C_suba_num0 = 2;	
					break;
				// �ӵ�ַΪ3���ֽ�
				case 3:
					I2C_suba_num0 = 3;	
					break;
				default:
				    break;
			}
			
			if((no == 1) && (I2C_suba_num0 == 1))
				I2C_state0 = STATE_WRITE_FINAL;	// ���ֽڵ�ַ��8��x�ṹ��ַ����1���ֽ�����
			else
				I2C_state0 = STATE_WRITE_NEXT;	// ���ֽڵ�ַ���Ͷ���ֽ�����	
								
			I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, I2C_sla0, false);		// ����д����										
		    I2C_suba_num0--;
		    I2CMasterDataPut(I2C0_MASTER_BASE, 
		    				(I2C_suba0 >> (8 * I2C_suba_num0)));			// д�ӵ�ַ���ֽ�
		    I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);// ͻ��д��������
		    
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
				// �ӵ�ַΪ��8+x������
				case 0:
					I2C_sla1 = I2C_sla1 | ((suba >> 8) & 0x07);
					I2C_suba_num1 = 1;
					break;	
				// �ӵ�ַΪ1���ֽ� 	
				case 1:
					I2C_suba_num1 = 1;	
					break;
				// �ӵ�ַΪ2���ֽ�
				case 2:
					I2C_suba_num1 = 2;	
					break;
				// �ӵ�ַΪ3���ֽ�
				case 3:
					I2C_suba_num1 = 3;	
					break;
				default:
					break;
			}
			
			if((no == 1) && (I2C_suba_num1 == 1))
				I2C_state1 = STATE_WRITE_FINAL;										// ���ֽڵ�ַ��8��x�ṹ��ַ����1���ֽ�����
			else
				I2C_state1 = STATE_WRITE_NEXT;										// ���ֽڵ�ַ���Ͷ���ֽ�����	
								
			I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C_sla1, false);				// ����д����										
		    I2C_suba_num1--;
		    I2CMasterDataPut(I2C1_MASTER_BASE, 
		    				(I2C_suba1 >> (8 * I2C_suba_num1)));					// д�ӵ�ַ���ֽ�
		    I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);	// ͻ��д��������
		    
			while(I2C_state1 != STATE_IDLE){ 
			    if(cnt++> I2CWAITIME){
					return (false);
				} 
		    }										//�Ƿ���Ҫ�����ź����ȴ���ʽ��������ʡcpu��Դ									

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
** �������ƣ�ISendStrExt()
** �������ܣ������ӵ�ַ�������Ͷ��ֽ����ݡ�
** ��ڲ�����sla		������ַ
**           s			Ҫ���͵�����
**  		 no			���͵��ֽڸ���
** ���ڲ���������ֵΪfalseʱ��ʾ��������Ϊtrueʱ��ʾ������ȷ��
** ˵����	 ���������ֽ�����2���ֽ�
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
			if (no > 1)								// ���͵��ֽڲ�������
			{
				if(no == 2)
					I2C_state0 = STATE_WRITE_FINAL;
				else
					I2C_state0 = STATE_WRITE_NEXT;
			
				I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, I2C_sla0, false);	// ����д����
		    	I2CMasterDataPut(I2C0_MASTER_BASE, *I2C_buf0++);
		    	I2C_num0--;
		    	I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);// ͻ��д��������
		        
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
			if (no > 1)								// ���͵��ֽڲ�������
			{
				if(no == 2)
					I2C_state1 = STATE_WRITE_FINAL;
				else
					I2C_state1 = STATE_WRITE_NEXT;
			
				I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C_sla1, false);	// ����д����
		    	I2CMasterDataPut(I2C1_MASTER_BASE, *I2C_buf1++);
		    	I2C_num1--;
		    	I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);// ͻ��д��������
		        
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
** �������ƣ�IRcvByte()
** �������ܣ������ӵ�ַ������ȡ1�ֽ����ݡ�
** ��ڲ�����sla		������ַ
**           c			�������ݵı���ָ��
** ���ڲ���������ֵΪfalseʱ��ʾ��������Ϊtrueʱ��ʾ������ȷ��
** ˵����	ʹ��ǰҪ��ʼ����I2C���Ź��ܺ�I2C�жϣ�����ʹ��I2C��ģʽ
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
			
			I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, I2C_sla0, true);	// ����������
			I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);// �������ζ�ȡ
			
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
			
			I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C_sla1, true);	// ����������
			I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);// �������ζ�ȡ
			
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
** �������� ��IRcvStr()
** �������� �������ӵ�ַ������ȡN�ֽ�����
** ��ڲ��� ��	sla			�����ӵ�ַ
**				suba_type	�ӵ�ַ�ṹ	0��8+X�ṹ  1�����ֽڵ�ַ  2��˫�ֽڵ�ַ  3�����ֽڵ�ַ
**			  	suba		�����ڲ������ַ
**			  	*s			��Ҫ��ȡ�����ݵ�ָ��
**			  	no			��Ҫ��ȡ�����ݵĸ���
** ���ڲ��� ��	TRUE		�����ɹ�
**			  	FALSE		����ʧ��
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
				// �ӵ�ַΪ��8+x������
				case 0:
					I2C_sla0 = I2C_sla0 | ((suba >> 8) & 0x07);
					I2C_suba_num0 = 1;
					break;	
				// �ӵ�ַΪ1���ֽ� 	
				case 1:
					I2C_suba_num0 = 1;	
					break;
				// �ӵ�ַΪ2���ֽ�
				case 2:
					I2C_suba_num0 = 2;	
					break;
				// �ӵ�ַΪ3���ֽ�
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
								
			I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, I2C_sla0, false);		// ����д����										
		    I2C_suba_num0--;
		    I2CMasterDataPut(I2C0_MASTER_BASE, 
		    				(I2C_suba0 >> (8 * I2C_suba_num0)));			// д�ӵ�ַ���ֽ�
		    I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);// ͻ��д��������
		    
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
				// �ӵ�ַΪ��8+x������
				case 0:
					I2C_sla1 = I2C_sla1 | ((suba >> 8) & 0x07);
					I2C_suba_num1 = 1;
					break;	
				// �ӵ�ַΪ1���ֽ� 	
				case 1:
					I2C_suba_num1 = 1;	
					break;
				// �ӵ�ַΪ2���ֽ�
				case 2:
					I2C_suba_num1 = 2;	
					break;
				// �ӵ�ַΪ3���ֽ�
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
								
			I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C_sla1, false);		// ����д����										
		    I2C_suba_num1--;
		    I2CMasterDataPut(I2C1_MASTER_BASE, 
		    				(I2C_suba1 >> (8 * I2C_suba_num1)));			// д�ӵ�ַ���ֽ�
		    I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_START);// ͻ��д��������
		    
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
** �������ƣ�IRvcStrExt()
** �������ܣ������ӵ�ַ������ȡN�ֽ����ݡ�
** ��ڲ�����sla		������ַ
**           s			�������ݵı���ָ��
**			 no			��Ҫ��ȡ�����ݵĸ���
** ���ڲ�����ʹ��ǰҪ��ʼ����I2C���Ź��ܺ�I2C�жϣ�����ʹ��I2C��ģʽ
** ˵����	����Ҫ��ȡ2���ֽ�
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
					
				I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, I2C_sla0, true);	// ����������
		    	I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);// ͻ������������
			
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
					
				I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C_sla1, true);	// ����������
		    	I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_RECEIVE_START);// ͻ������������
			
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
** �������� ��I2C_ISR()
** �������� ���ж϶�д����
** ��ڲ��� ����
** ���ڲ��� ����
*******************************************************************************************/
void I2C0IntHandler (void)
{
    I2CMasterIntClear(I2C0_MASTER_BASE);		// ���I2C�жϱ�־

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
        // ����״̬
        case STATE_IDLE:
        {
            break;
        }
        // д�����һ���ֽ�״̬
        case STATE_WRITE_FINAL_END:
        {
        	I2C_state0 = STATE_IDLE;
        	break;
        }

        // д�굥���ֽ�״̬
        case STATE_WRITE_ONE:
        {
        	I2C_state0 = STATE_IDLE;
        	break;
        }
		// д��һ������
        case STATE_WRITE_NEXT:
        {
        	// ����һ���ֽ�д��Ĵ���
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
            // ����ִ�п�д����(run=1)
            I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
            break;
        }
        // д���һ������
        case STATE_WRITE_FINAL:
        {
            I2CMasterDataPut(I2C0_MASTER_BASE, *I2C_buf0);
            I2C_num0--;
            // ����ֹͣ
            I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
			// ��һ��״̬Ϊ��д���
            I2C_state0= STATE_IDLE;
            break;
        }
 
 		// ��ȡһ���ֽ�
        case STATE_READ_ONE:
        {

            I2CMasterSlaveAddrSet(I2C0_MASTER_BASE, I2C_sla0, true);

            I2CMasterControl(I2C0_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

            I2C_state0= STATE_READ_WAIT;

            break;
        }
		// ��ȡ����ֽڿ�ʼ
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
		// ��ȡ��һ���ֽ�
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
		// ��ȡ���һ���ֽ�
        case STATE_READ_FINAL:
        {

            *I2C_buf0++ = I2CMasterDataGet(I2C0_MASTER_BASE);
            I2C_num0--;

            I2CMasterControl(I2C0_MASTER_BASE,
                             I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

            I2C_state0= STATE_READ_WAIT;
            break;
        }
		// �ȴ���ȡһ���ֽ�
        case STATE_READ_WAIT:
        {
 
            *I2C_buf0++  = I2CMasterDataGet(I2C0_MASTER_BASE); // ��ȡ����
            I2C_num0--;

            I2C_state0= STATE_IDLE;
            break;
        }
    }
}
/*******************************************************************************************
** �������� ��I2C_ISR()
** �������� ���ж϶�д����
** ��ڲ��� ����
** ���ڲ��� ����
*******************************************************************************************/
void I2C1IntHandler (void)
{
    I2CMasterIntClear(I2C1_MASTER_BASE);		// ���I2C�жϱ�־

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
        // ����״̬
        case STATE_IDLE:
        {
            break;
        }
        // д�����һ���ֽ�״̬
        case STATE_WRITE_FINAL_END:
        {
        	I2C_state1 = STATE_IDLE;
        	break;
        }
        // д�굥���ֽ�״̬
        case STATE_WRITE_ONE:
        {
        	I2C_state1 = STATE_IDLE;
        	break;
        }
		// д��һ������
        case STATE_WRITE_NEXT:
        {
        	// ����һ���ֽ�д��Ĵ���
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
            // ����ִ�п�д����(run=1)
            I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_CONT);
            break;
        }
        // д���һ������
        case STATE_WRITE_FINAL:
        {
            I2CMasterDataPut(I2C1_MASTER_BASE, *I2C_buf1);
            I2C_num1--;
            // ����ֹͣ
            I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_BURST_SEND_FINISH);
			// ��һ��״̬Ϊ��д���
            I2C_state1= STATE_WRITE_FINAL_END;
            break;
        }
 
 		// ��ȡһ���ֽ�
        case STATE_READ_ONE:
        {

            I2CMasterSlaveAddrSet(I2C1_MASTER_BASE, I2C_sla1, true);

            I2CMasterControl(I2C1_MASTER_BASE, I2C_MASTER_CMD_SINGLE_RECEIVE);

            I2C_state1= STATE_READ_WAIT;

            break;
        }
		// ��ȡ����ֽڿ�ʼ
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
		// ��ȡ��һ���ֽ�
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
		// ��ȡ���һ���ֽ�
        case STATE_READ_FINAL:
        {
            *I2C_buf1++ = I2CMasterDataGet(I2C1_MASTER_BASE);
            I2C_num1--;

            I2CMasterControl(I2C1_MASTER_BASE,
                             I2C_MASTER_CMD_BURST_RECEIVE_FINISH);

            I2C_state1= STATE_READ_WAIT;
            break;
        }
		// �ȴ���ȡһ���ֽ�
        case STATE_READ_WAIT:
        {
 
            *I2C_buf1++  = I2CMasterDataGet(I2C1_MASTER_BASE); // ��ȡ����
            I2C_num1--;

            I2C_state1= STATE_IDLE;
            break;
        }
    }
}
