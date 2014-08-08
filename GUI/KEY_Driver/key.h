//KEY_2.0 Designed by Mei_Lin at 20101224

#ifndef KEY_H_H
#define KEY_H_H

#include "key_msg_def.h"

extern INT8S key_init (void);
extern INT32U key_msg_get (void);
extern INT8S key_msg_post (INT32U msg);//向按键消息队列发送消息(作用:可作虚拟按键用,比如,串口虚拟按键)
extern void key_msg_flush (void);//清空按键消息队列(作用:从一个控件切换到另一个控件之前应先清空消息队列,防止后者响应前者的残留消息)

#endif

/*
//sample: 菜单控件框架

#define KEY_UP     KEY_1_DOWN 
#define KEY_DOWN   KEY_2_DOWN 
#define KEY_ENTER  KEY_3_DOWN 
#define KEY_RETURN KEY_4_DOWN 

void menu_main (void)
{
    //菜单初始化
    menu_init ();
    
    //消息循环
    while (1)
    {
        //菜单刷新
        menu_refresh ();
        
        switch (key_msg_get ())
        {
            //上移
            case KEY_UP:
                menu_up ();   
                break;
            
            //下移
            case KEY_DOWN:
                menu_down ();
                break;
            
            //确认
            case KEY_ENTER:
                menu_enter ();      //进入子菜单，或者进入另一控件(如时间设置控件，或者密码控件，<一切皆控件, 控件都有消息循环, 当前控件独享按键消息!!!>)
                break;
                
            //返回    
            case KEY_RETURN:
                menu_return ();     //返回上一级菜单，或者进入另一控件(如桌面控件，<一切皆控件, 控件都有消息循环, 当前控件独享按键消息!!!>)
                break;
                
            default:
                break;   
        }   
    }      
}
*/


