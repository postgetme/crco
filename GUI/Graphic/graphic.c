#include "includes.h"

void draw_bitmap (INT8U x0, INT8U y0, INT8U width, INT8U high, const INT8U* data)
{
    INT8U i;
    INT8U j;

	INT8U line = high >> 3;

	for(i = 0; i < line; i++)
	{		
	    for (j = 0; j < width; j++)
	    {
	        lcd_write_byte(x0 + j, y0 + i, *data++);   
	    }
	}  
}

void draw_line (INT8U x0, INT8U y0, INT8U x1, INT8U y1, INT8U mode)
{
   INT16S   dx;						// ֱ��x���ֵ����
   INT16S   dy;          			// ֱ��y���ֵ����
   INT16S    dx_sym;					// x����������Ϊ-1ʱ��ֵ����Ϊ1ʱ��ֵ����
   INT16S    dy_sym;					// y����������Ϊ-1ʱ��ֵ����Ϊ1ʱ��ֵ����
   INT8U   dx_x2;					// dx*2ֵ���������ڼӿ������ٶ�
   INT8U   dy_x2;					// dy*2ֵ���������ڼӿ������ٶ�
   INT16S   di;						// ���߱���
   
   
   dx = x1-x0;						// ��ȡ����֮��Ĳ�ֵ
   dy = y1-y0;
   
   /* �ж��������򣬻��Ƿ�Ϊˮƽ�ߡ���ֱ�ߡ��� */
   if(dx>0)							// �ж�x�᷽��
   {  dx_sym = 1;					// dx>0������dx_sym=1
   }
   else
   {  if(dx<0)
      {  dx_sym = -1;				// dx<0������dx_sym=-1
      }
      else
      {  // dx==0������ֱ�ߣ���һ��
         lcd_v_line(x0, y0, y1, mode);
      	 return;
      }
   }
   
   if(dy>0)							// �ж�y�᷽��
   {  dy_sym = 1;					// dy>0������dy_sym=1
   }
   else
   {  if(dy<0)
      {  dy_sym = -1;				// dy<0������dy_sym=-1
      }
      else
      {  // dy==0����ˮƽ�ߣ���һ��
         lcd_h_line(x0, y0, x1, mode);
      	 return;
      }
   }
    
   /* ��dx��dyȡ����ֵ */
   dx = dx_sym * dx;
   dy = dy_sym * dy;
 
   /* ����2����dx��dyֵ */
   dx_x2 = dx*2;
   dy_x2 = dy*2;
   
   /* ʹ��Bresenham�����л�ֱ�� */
   if(dx>=dy)						// ����dx>=dy����ʹ��x��Ϊ��׼
   {  di = dy_x2 - dx;
      while(x0!=x1)
      {  lcd_write_dot(x0, y0, mode);
         x0 += dx_sym;
         if(di<0)
         {  di += dy_x2;			// �������һ���ľ���ֵ
         }
         else
         {  di += dy_x2 - dx_x2;
            y0 += dy_sym;
         }
      }
      lcd_write_dot(x0, y0, mode);		// ��ʾ���һ��
   }
   else								// ����dx<dy����ʹ��y��Ϊ��׼
   {  di = dx_x2 - dy;
      while(y0!=y1)
      {  lcd_write_dot(x0, y0, mode);
         y0 += dy_sym;
         if(di<0)
         {  di += dx_x2;
         }
         else
         {  di += dx_x2 - dy_x2;
            x0 += dx_sym;
         }
      }
      lcd_write_dot(x0, y0, mode);		// ��ʾ���һ��
   } 
   
}

void draw_rectangle (INT8U x0, INT8U y0, INT8U x1, INT8U y1, INT8U mode)
{
   lcd_h_line(x0, y0, x1, mode);
   lcd_h_line(x0, y1, x1, mode);
   lcd_v_line(x0, y0, y1, mode);
   lcd_v_line(x1, y0, y1, mode);    
}

void draw_rectangle_fill (INT8U x0, INT8U y0, INT8U x1, INT8U y1, INT8U mode)
{
INT8U  i;

   /* ���ҳ��������Ͻ������½ǵ������㣬������(x0,y0)��(x1,y1) */
   if(x0>x1) 						// ��x0>x1����x0��x1����
   {  i = x0;
      x0 = x1;
      x1 = i;
   }
   if(y0>y1)						// ��y0>y1����y0��y1����
   {  i = y0;
      y0 = y1;
      y1 = i;
   }
   
   /* �ж��Ƿ�ֻ��ֱ�� */
   if(y0==y1) 
   {  lcd_h_line(x0, y0, x1, mode);
      return;
   }
   if(x0==x1) 
   {  lcd_v_line(x0, y0, y1, mode);
      return;
   }

   while(x0<=x1)						
   {  lcd_v_line(x0, y0, y1, mode);	// ��ǰ��ˮƽ��
      x0++;							// ��һ��
   }    
}
