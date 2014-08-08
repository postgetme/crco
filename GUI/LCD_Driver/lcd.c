#include "includes.h"

#define CSLCD_PERIPH				SYSCTL_PERIPH_GPIOC
#define RD_PERIPH						SYSCTL_PERIPH_GPIOD
#define WR_PERIPH						SYSCTL_PERIPH_GPIOD
#define RS_PERIPH						SYSCTL_PERIPH_GPIOG
#define CSLED_PERIPH				SYSCTL_PERIPH_GPIOC
#define LEDBL_PERIPH				SYSCTL_PERIPH_GPIOC
#define CSKEY_PERIPH				SYSCTL_PERIPH_GPIOC

#define	CSLCD_GPIO_BASE			GPIO_PORTC_BASE			
#define	RD_GPIO_BASE				GPIO_PORTD_BASE
#define	WR_GPIO_BASE				GPIO_PORTD_BASE
#define	RS_GPIO_BASE				GPIO_PORTG_BASE
#define	CSLED_GPIO_BASE			GPIO_PORTC_BASE
#define	LEDBL_GPIO_BASE			GPIO_PORTC_BASE
#define CSKEY_GPIO_BASE			GPIO_PORTC_BASE

#define LCM_CSLCD         	GPIO_PIN_6				
#define LCM_RD	         		GPIO_PIN_1			
#define LCM_WR         			GPIO_PIN_0			
#define LCM_RS         			GPIO_PIN_2			
#define	LCM_CSLED						GPIO_PIN_4	
#define	LCM_LEDBL						GPIO_PIN_5				
#define LCM_CSKEY						GPIO_PIN_7

static OS_EVENT* lcd_write_sem;

static void	__CSLCD__	(char data);
static void	__RD__	(char data);
static void	__WR__	(char data);
static void	__RS__	(char data);
static void	__CSLED__	(char data); 
	
static void 	LCM_WRITE_DAT(INT8U data);	 
static INT8U	LCM_READ_DAT(void);
static void 	LCM_PORT_INI(void);

static void 	lcd_busy(void) ;
static void 	lcd_write_cmd(INT8U cmd);
static void 	lcd_write_dat(INT8U dat);

#define  GUI_LCM_XMAX		128						
#define  GUI_LCM_YMAX		64	
/* 定义显示缓冲区 */
INT8U  gui_disp_buf[GUI_LCM_YMAX/8][GUI_LCM_XMAX];
INT8U const  DEC_HEX_TAB[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

static void __CSLCD__(char data)
{
	if(0 == data){	
		GPIOPinWrite(CSLCD_GPIO_BASE, LCM_CSLCD, 0);
	}else{
		GPIOPinWrite(CSLCD_GPIO_BASE, LCM_CSLCD, LCM_CSLCD);		
	}

}

static void __RD__(char data)
{
	if(0 == data)	{
		GPIOPinWrite(RD_GPIO_BASE, LCM_RD, 0);		
	}else{
		GPIOPinWrite(RD_GPIO_BASE, LCM_RD, LCM_RD);
	}
}	

static void __WR__(char data)
{
	if(data==0)	{
		GPIOPinWrite(WR_GPIO_BASE, LCM_WR, 0);	  	
	}else		{
		GPIOPinWrite(WR_GPIO_BASE, LCM_WR, LCM_WR);		
	}

} 

static void __RS__(char data)
{
	if(0 == data){	
		GPIOPinWrite(RS_GPIO_BASE, LCM_RS, 0); 	
	}else{		
		GPIOPinWrite(RS_GPIO_BASE, LCM_RS, LCM_RS);	
	}
}	 

static void __CSLED__(char data)
{
	if(0 == data){	
		GPIOPinWrite(CSLED_GPIO_BASE, LCM_CSLED, 0);
	}else{
		GPIOPinWrite(CSLED_GPIO_BASE, LCM_CSLED, LCM_CSLED);		
	}
}

static void __CSKEY__(char data)
{
	if(0 == data){	
		GPIOPinWrite(CSKEY_GPIO_BASE, LCM_CSKEY, 0);
	}else{
		GPIOPinWrite(CSKEY_GPIO_BASE, LCM_CSKEY, LCM_CSKEY);		
	}
}

static void LCM_WRITE_DAT(INT8U data)	
{
	GPIOPinWrite(GPIO_PORTF_BASE, 0xff, data);
}

static INT8U LCM_READ_DAT(void) 
{
	INT8U data = 0;
	
	SysCtlPeripheralEnable(GPIO_PORTF_BASE);		
	GPIOPadConfigSet(GPIO_PORTF_BASE, 0xff, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
	GPIODirModeSet(GPIO_PORTF_BASE, 0xff, GPIO_DIR_MODE_IN);
	
	data = GPIOPinRead(GPIO_PORTF_BASE, 0xff);

	// 恢复IO口的输出设置	
	GPIODirModeSet(GPIO_PORTF_BASE, 0xff,GPIO_DIR_MODE_OUT);
	GPIOPadConfigSet(GPIO_PORTF_BASE, 0xff, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU); 	

	return data;  
}	  

void LCM_PORT_INI(void)
{
    SysCtlPeripheralEnable(CSLCD_PERIPH);	
	SysCtlPeripheralEnable(RD_PERIPH);	  	
	SysCtlPeripheralEnable(WR_PERIPH);	  	
	SysCtlPeripheralEnable(RS_PERIPH);	 	
	SysCtlPeripheralEnable(CSLED_PERIPH);	
 	SysCtlPeripheralEnable(LEDBL_PERIPH);	
 	SysCtlPeripheralEnable(CSKEY_PERIPH);					   
	
	GPIODirModeSet(CSLCD_GPIO_BASE, LCM_CSLCD,GPIO_DIR_MODE_OUT);			  
	GPIODirModeSet(RD_GPIO_BASE, 	LCM_RD,GPIO_DIR_MODE_OUT);			  
	GPIODirModeSet(WR_GPIO_BASE, 	LCM_WR,	GPIO_DIR_MODE_OUT);			  
	GPIODirModeSet(RS_GPIO_BASE, 	LCM_RS,	GPIO_DIR_MODE_OUT);			  
	GPIODirModeSet(CSLED_GPIO_BASE, LCM_CSLED,	GPIO_DIR_MODE_OUT);
 	GPIODirModeSet(LEDBL_GPIO_BASE, LCM_LEDBL,	GPIO_DIR_MODE_OUT); 
	GPIODirModeSet(CSKEY_GPIO_BASE, LCM_CSKEY,	GPIO_DIR_MODE_OUT); 

    
	GPIOPadConfigSet(CSLCD_GPIO_BASE, LCM_CSLCD, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(RD_GPIO_BASE, LCM_RD, GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(WR_GPIO_BASE, 	LCM_WR,	 GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(RS_GPIO_BASE, 	LCM_RS,	 GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
	GPIOPadConfigSet(CSLED_GPIO_BASE, 	LCM_CSLED,	 GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
 	GPIOPadConfigSet(LEDBL_GPIO_BASE, 	LCM_LEDBL,	 GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);   
	GPIOPadConfigSet(CSKEY_GPIO_BASE, 	LCM_CSKEY,	 GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);     
  
	//数据	
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIOPadConfigSet(GPIO_PORTF_BASE, 0xff, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);	
	GPIODirModeSet(GPIO_PORTF_BASE, 0xff, GPIO_DIR_MODE_OUT); 

	GPIOPinWrite(CSKEY_GPIO_BASE, LCM_CSKEY, LCM_CSKEY);

	lcd_write_sem = OSSemCreate (1);
} 

static void LCD_write_byte(INT8U x, INT8U y, INT8U dd)
{		
    INT8U err;
    
    OSSemPend (lcd_write_sem, 0, &err);
    
        lcd_write_cmd(0xb0 | y); 		// select page 0~7
        lcd_write_cmd(0x10|(x>>4));     // start form column 0
        lcd_write_cmd(0x00|(x&0x0f));	// (2byte command)
        lcd_busy();					
        lcd_write_dat(dd);
        
    OSSemPost (lcd_write_sem);						
}

static void LCM_BackLightControl(INT8U mode)
{
	if(0 == mode){	
		GPIOPinWrite(LEDBL_GPIO_BASE, LCM_LEDBL, 0);
	}else{
		GPIOPinWrite(LEDBL_GPIO_BASE, LCM_LEDBL, LCM_LEDBL);
	}
}

static void LEDLightCtl(INT8U led_state)  
{ 
    INT8U err;
    
    OSSemPend (lcd_write_sem, 0, &err);

    	__CSLCD__(1);
    	__CSLED__(1);
    	LCM_WRITE_DAT (led_state);
    	__CSLED__(0);
       	__CSLED__(1);
	
    OSSemPost (lcd_write_sem);				
}	

static void LCM_Cls(void)
{
	INT8U Page = 0;
	INT8U Col  = 0;

	for(Page=0;Page<8;Page++) {	 
		for(Col=0;Col<128;Col++) {
			   lcd_write_byte(Col,Page,0);		  
		} 
	}	
}  

static void LCM_Init(void)
{	
	LCM_PORT_INI();

	lcd_write_cmd(0xA2);					//lcd bais= 1/9
	OSTimeDly(1);
	lcd_write_cmd(0xA0);					//ADC= normal
	OSTimeDly(1);
	lcd_write_cmd(0xC8);					//SHL= reverse	
	OSTimeDly(1);
	lcd_write_cmd(0x40);					//init line = 0
	OSTimeDly(1);
	lcd_write_cmd(0x2C);					//
	OSTimeDly(5);
	lcd_write_cmd(0x2E);	
	OSTimeDly(5);
	lcd_write_cmd(0x2F);					//turn on power	
	OSTimeDly(5);

	lcd_write_cmd(0xF8);					//booster ratio set: X4
	OSTimeDly(1);
	lcd_write_cmd(0x00);						
	OSTimeDly(1);

	lcd_write_cmd(0x81);					//set contrsat
	OSTimeDly(1);
	lcd_write_cmd(0x2F);
	OSTimeDly(1);

	lcd_write_cmd(0xAF);					//turn on lcd
	OSTimeDly(1);
		
	lcd_write_cmd(0xB0);					//set y
	OSTimeDly(1);

	lcd_write_cmd(0x10);					//set x
	OSTimeDly(1);
	lcd_write_cmd(0x00);
	OSTimeDly(1);
		
/***********为防止上电时电源抖动需初始化两次*****************/	
	LCM_Cls();	
}

static void lcd_write_cmd(INT8U cmd) 
{
	__RS__(0);		//指令
	__RD__(1);		
	__WR__(1);
	LCM_WRITE_DAT(cmd);
	__CSLCD__(0);
	__WR__(0);
	__WR__(1);
	__CSLCD__(1);
}

static void lcd_write_dat(INT8U dat) 
{	
	__RS__(1);		//数据
	__RD__(1);		
	__WR__(1);
	LCM_WRITE_DAT(dat);
	__CSLCD__(0);
	__WR__(0);
	__WR__(1);
	__CSLCD__(1);
}

static void lcd_busy(void) 
{
	INT8U ii= 0; 	
	__RS__(0);		//读
	__WR__(1);		
	__CSLCD__(0);
			
	while(1) {
		__RD__(0);
	 	ii = (LCM_READ_DAT()&0x80);
		if (!ii)  break;
		__RD__(1);	 
	}	 	
	__CSLCD__(1);
	__RS__(1);
}

void lcd_init (void)
{
    LCM_Init ();    
	memset (gui_disp_buf, 0, sizeof (gui_disp_buf));

	lcd_light (1);
	OSTimeDly (100);
}

void lcd_clear (void)
{
    LCM_Cls ();
}

void lcd_write_dot (INT8U x, INT8U y, INT8U dat)
{
	INT8U   bak;
   
   // 设置相应的点为1或0 
   bak = lcd_read_byte(x,y/8);
   if(0==dat)
   {  bak &= (~DEC_HEX_TAB[y&0x07]);
   }
   else
   {  bak |= DEC_HEX_TAB[y&0x07];
   }
  
   // 刷新显示 
   lcd_write_byte(x, y/8, bak);
}

INT8U lcd_read_dot (INT8U x, INT8U y)
{
   INT8U bak;
   bak = lcd_read_byte(x,y);
   if( (bak & (DEC_HEX_TAB[y&0x07])) == 0 ) return 0x00;
     else  return 0x01;    
}

void lcd_write_byte (INT8U x, INT8U y, INT8U dat)
{
   LCD_write_byte (x, y, dat);  
   gui_disp_buf[y][x] = dat; 
}

INT8U lcd_read_byte (INT8U x, INT8U y)
{
    return(gui_disp_buf[y][x]);
}

void lcd_h_line (INT8U x0, INT8U y0, INT8U x1, INT8U mode)
{
  INT8U  bak;

   if(x0>x1) 						// 对x0、x1大小进行排列，以便画图
   {  bak = x1;
      x1 = x0;
      x0 = bak;
   }
   
   do
   {  lcd_write_dot(x0, y0, mode);		// 逐点显示，描出垂直线
      x0++;
   }while(x1>=x0);
}

void lcd_v_line (INT8U x0, INT8U y0, INT8U y1, INT8U mode)
{
#if 1
 INT8U  bak;
   INT8U  wr_dat;
  
   if(y0>y1) 		// 对y0、y1大小进行排列，以便画图
   {  bak = y1;
      y1 = y0;
      y0 = bak;
   }
   
   do
   {  // 先读取当前点的字节数据
      bak = lcd_read_byte(x0,y0>>3);	
      
      // 进行'与'/'或'操作后，将正确的数据写回LCM
      // 若y0和y1不是同一字节，则y0--当前字节结束，即(y0+8)&0x38，全写1，或者0。
      // 若y0和y1是同一字节，则y0--y1，要全写1，或者0。
      // 方法：dat=0xff，然后按y0清零dat低位，按y1清零高位。
      if((y0>>3) != (y1>>3))		// 竖直线是否跨越两个字节(或以上)
      {  wr_dat = 0xFF << (y0&0x07);// 清0低位
      
         if(mode)					
         {  wr_dat = bak | wr_dat;	// 若color不为0，则显示
         }
         else
         {  wr_dat = ~wr_dat;		// 若color为0，则清除显示
            wr_dat = bak & wr_dat;
         }
         lcd_write_byte(x0,y0/8, wr_dat);
         y0 = (y0+8)&0x38;
      }
      else
      {  wr_dat = 0xFF << (y0&0x07);
         wr_dat = wr_dat &  ( 0xFF >> (7-(y1&0x07)) );
              
         if(mode)					
         {  wr_dat = bak | wr_dat;	// 若color不为0，则显示
         }
         else
         {  wr_dat = ~wr_dat;		// 若color为0，则清除显示
            wr_dat = bak & wr_dat;
         }
         lcd_write_byte(x0,y0>>3, wr_dat);
         return;
      } // end of if((y0>>3) != (y1>>3))... else...
   }while(y1>=y0);
#endif


//	INT8U  bak;
//
//   if(y0>y1) 						// 对x0、x1大小进行排列，以便画图
//   {  bak = y1;
//      y1 = y0;
//      y0 = bak;
//   }
//   
//   do
//   {  lcd_write_dot(x0, y0, mode);		// 逐点显示，描出垂直线
//      y0++;
//   }while(y1>=y0);  
}

void lcd_light (INT8U on_off)
{
    LCM_BackLightControl (on_off);
}

INT8U lcd_read_key (void)
{
    INT8U err;
    INT8U temp;
    
    OSSemPend (lcd_write_sem, 0, &err);

    	__CSLCD__(1);
    	__CSKEY__(0);
    	
    	temp = LCM_READ_DAT ();
    	
       	__CSKEY__(1);
	
    OSSemPost (lcd_write_sem);	  
    
    return temp;  
}

void lcd_write_led (INT8U dat)
{
    LEDLightCtl (dat);
}
