#include "includes.h"
#include "zk.h"	

static void PutHZ_8_(INT8U x, INT8U y, INT8U *data, INT8U reverse)
{
	INT8U i=0;
	INT8U *pd =data;
	INT8U rev=reverse;	 // ���Ա�־

	for(i=0;i<8;i++) {
		if(rev==0) {
			lcd_write_byte(x+i, y, *pd++);
		}else{
			lcd_write_byte(x+i, y, ~(*pd++));
		}		
	}

	for(i=0;i<8;i++) {
		if(rev==0){ 	
			lcd_write_byte(x+i, y+1, *pd++);
		}else{		
			lcd_write_byte(x+i, y+1, ~(*pd++));
		}
	}
}

static void PutHZ_16_(INT8U x, INT8U y, INT8U *data, INT8U reverse)
{	
	INT8U i   = 0;
	INT8U *pd = data;
	INT8U rev = reverse;	 // ���Ա�־
//	INT8U err;


	for(i=0;i<16;i++) {	 
		if(rev==0){ 	
			lcd_write_byte(x+i, y, *pd++);
		}else{
			lcd_write_byte(x+i, y, ~(*pd++));
		}
	}		
				 	
	for(i=0;i<16;i++) {
		if(rev==0){	
			lcd_write_byte(x+i, y+1, *pd++);
		}else{		
			lcd_write_byte(x+i, y+1, ~(*pd++));
		}
	}
}

static INT8S LCM_Write(INT8U x, INT8U y, INT8U* data, INT8U mode)
{ 
	INT16U j;
	INT16U xx = x,yy = y;
	char* temp = (char*)data;
	INT8U  rev  = mode;

	for (;*temp != '\0';) {
		// ���ֵ�ASCII���Ǵ���0x80		
		if(*temp < 0x7f) {
		// ��ʾӢ���ַ�	
			for(j=0; j<HZ16_16_MAX_SIZE;) {
				if(*temp == EN8_16_TAB[j].index) {
					if (xx < 128) {
						// �ҵ�
						PutHZ_8_(xx, yy, (INT8U*)EN8_16_TAB[j].zm, rev);
						xx+=8;		// X�����ƶ�һ���ֽڿ��				
						temp++;															
						j = 0;
					}else {
						xx = 0;
						yy = yy+2;
					}
					break;
				}					
				j++;		
			}
			//���û���ҵ����ֿ����˳�	
			if(j==HZ16_16_MAX_SIZE)	
				temp++;
		}else {	
		// ��ʾ����	
			for (j = 0; j < HZ16_16_MAX_SIZE;) {			
				if (*temp == FONT16_16_TAB[j].index[0])	{
					if (*(temp+1) == FONT16_16_TAB[j].index[1])	{
						if (xx < 128) {
							// �ҵ�����
							PutHZ_16_(xx,yy,(INT8U*)FONT16_16_TAB[j].zm,rev); 
							xx+=16;	// 16*16�ͺ��ֿ��	
							temp++,temp++; 					
							j = 0;
						}else {
							xx = 0;
							yy = yy+2;
						}
						break;			
					}
				}
				j++;
			}//endfor 			
			// ���û���ҵ����ֿ����˳�
			if(j==HZ16_16_MAX_SIZE)	{
				temp++;
				temp++;
			}	
		}//endif
	}//endfor	   

	return 1;
}

void font_print (INT8U x, INT8U y, INT8U* str, INT8U mode)
{
    LCM_Write (x, y, str, mode);
}

void font_printf (INT8U x, INT8U y, INT8U mode, char* fmt, ...)
{
    va_list argptr;
    static char s[128];
    INT8U len;

    va_start (argptr, fmt);
    len = vsnprintf (s, sizeof (s), fmt, argptr);
    va_end (argptr);

	s[len] = 0;

	font_print (x, y, s, mode);
}


