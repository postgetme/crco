#ifndef WIDGET_EDIT_H_H
#define WIDGET_EDIT_H_H

typedef struct
{
    INT8U x;            //x坐标		  x=255,右对齐   x=254,居中
    INT8U y;            //y坐标		
    char* expr;         //表达示，"[0-5000]"/"(是|否)"
    char* format_expr;  //格式表达式，"%04d毫安"/"%s"
    FP32 step_val;      //步进值
    FP32 start_val;     //起始值
    INT8U style;        //风格，风格1，上下键调值，风格2，左右键调值    
}EDIT_INFO;

typedef struct
{
    INT32U msg;
    FP32 ret;   
}EDIT_RET;

extern void edit_main (void* pdata);

#endif

