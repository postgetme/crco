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
   INT16S   dx;						// 直线x轴差值变量
   INT16S   dy;          			// 直线y轴差值变量
   INT16S    dx_sym;					// x轴增长方向，为-1时减值方向，为1时增值方向
   INT16S    dy_sym;					// y轴增长方向，为-1时减值方向，为1时增值方向
   INT8U   dx_x2;					// dx*2值变量，用于加快运算速度
   INT8U   dy_x2;					// dy*2值变量，用于加快运算速度
   INT16S   di;						// 决策变量
   
   
   dx = x1-x0;						// 求取两点之间的差值
   dy = y1-y0;
   
   /* 判断增长方向，或是否为水平线、垂直线、点 */
   if(dx>0)							// 判断x轴方向
   {  dx_sym = 1;					// dx>0，设置dx_sym=1
   }
   else
   {  if(dx<0)
      {  dx_sym = -1;				// dx<0，设置dx_sym=-1
      }
      else
      {  // dx==0，画垂直线，或一点
         lcd_v_line(x0, y0, y1, mode);
      	 return;
      }
   }
   
   if(dy>0)							// 判断y轴方向
   {  dy_sym = 1;					// dy>0，设置dy_sym=1
   }
   else
   {  if(dy<0)
      {  dy_sym = -1;				// dy<0，设置dy_sym=-1
      }
      else
      {  // dy==0，画水平线，或一点
         lcd_h_line(x0, y0, x1, mode);
      	 return;
      }
   }
    
   /* 将dx、dy取绝对值 */
   dx = dx_sym * dx;
   dy = dy_sym * dy;
 
   /* 计算2倍的dx及dy值 */
   dx_x2 = dx*2;
   dy_x2 = dy*2;
   
   /* 使用Bresenham法进行画直线 */
   if(dx>=dy)						// 对于dx>=dy，则使用x轴为基准
   {  di = dy_x2 - dx;
      while(x0!=x1)
      {  lcd_write_dot(x0, y0, mode);
         x0 += dx_sym;
         if(di<0)
         {  di += dy_x2;			// 计算出下一步的决策值
         }
         else
         {  di += dy_x2 - dx_x2;
            y0 += dy_sym;
         }
      }
      lcd_write_dot(x0, y0, mode);		// 显示最后一点
   }
   else								// 对于dx<dy，则使用y轴为基准
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
      lcd_write_dot(x0, y0, mode);		// 显示最后一点
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

   /* 先找出矩形左上角与右下角的两个点，保存在(x0,y0)，(x1,y1) */
   if(x0>x1) 						// 若x0>x1，则x0与x1交换
   {  i = x0;
      x0 = x1;
      x1 = i;
   }
   if(y0>y1)						// 若y0>y1，则y0与y1交换
   {  i = y0;
      y0 = y1;
      y1 = i;
   }
   
   /* 判断是否只是直线 */
   if(y0==y1) 
   {  lcd_h_line(x0, y0, x1, mode);
      return;
   }
   if(x0==x1) 
   {  lcd_v_line(x0, y0, y1, mode);
      return;
   }

   while(x0<=x1)						
   {  lcd_v_line(x0, y0, y1, mode);	// 当前画水平线
      x0++;							// 下一行
   }    
}
