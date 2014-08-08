#ifndef WIDGET_EDIT_H_H
#define WIDGET_EDIT_H_H

typedef struct
{
    INT8U x;            //x����		  x=255,�Ҷ���   x=254,����
    INT8U y;            //y����		
    char* expr;         //���ʾ��"[0-5000]"/"(��|��)"
    char* format_expr;  //��ʽ���ʽ��"%04d����"/"%s"
    FP32 step_val;      //����ֵ
    FP32 start_val;     //��ʼֵ
    INT8U style;        //��񣬷��1�����¼���ֵ�����2�����Ҽ���ֵ    
}EDIT_INFO;

typedef struct
{
    INT32U msg;
    FP32 ret;   
}EDIT_RET;

extern void edit_main (void* pdata);

#endif

