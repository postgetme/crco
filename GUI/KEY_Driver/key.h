//KEY_2.0 Designed by Mei_Lin at 20101224

#ifndef KEY_H_H
#define KEY_H_H

#include "key_msg_def.h"

extern INT8S key_init (void);
extern INT32U key_msg_get (void);
extern INT8S key_msg_post (INT32U msg);//�򰴼���Ϣ���з�����Ϣ(����:�������ⰴ����,����,�������ⰴ��)
extern void key_msg_flush (void);//��հ�����Ϣ����(����:��һ���ؼ��л�����һ���ؼ�֮ǰӦ�������Ϣ����,��ֹ������Ӧǰ�ߵĲ�����Ϣ)

#endif

/*
//sample: �˵��ؼ����

#define KEY_UP     KEY_1_DOWN 
#define KEY_DOWN   KEY_2_DOWN 
#define KEY_ENTER  KEY_3_DOWN 
#define KEY_RETURN KEY_4_DOWN 

void menu_main (void)
{
    //�˵���ʼ��
    menu_init ();
    
    //��Ϣѭ��
    while (1)
    {
        //�˵�ˢ��
        menu_refresh ();
        
        switch (key_msg_get ())
        {
            //����
            case KEY_UP:
                menu_up ();   
                break;
            
            //����
            case KEY_DOWN:
                menu_down ();
                break;
            
            //ȷ��
            case KEY_ENTER:
                menu_enter ();      //�����Ӳ˵������߽�����һ�ؼ�(��ʱ�����ÿؼ�����������ؼ���<һ�нԿؼ�, �ؼ�������Ϣѭ��, ��ǰ�ؼ���������Ϣ!!!>)
                break;
                
            //����    
            case KEY_RETURN:
                menu_return ();     //������һ���˵������߽�����һ�ؼ�(������ؼ���<һ�нԿؼ�, �ؼ�������Ϣѭ��, ��ǰ�ؼ���������Ϣ!!!>)
                break;
                
            default:
                break;   
        }   
    }      
}
*/


