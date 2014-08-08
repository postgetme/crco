#ifndef WIDGET_H_H
#define WIDGET_H_H

#define WIDGET_CHINESE  0
#define WIDGET_ENGLISH  1

#define KEY_UP     KEY_2_DOWN 
#define KEY_DOWN   KEY_6_DOWN 
#define KEY_LEFT   KEY_5_DOWN 
#define KEY_RIGHT  KEY_7_DOWN 
#define KEY_ENTER  KEY_3_DOWN 
#define KEY_RETURN KEY_1_DOWN

#define KEY_UP_UP     KEY_2_UP 
#define KEY_DOWN_UP   KEY_6_UP 
#define KEY_LEFT_UP   KEY_5_UP 
#define KEY_RIGHT_UP  KEY_7_UP 
#define KEY_ENTER_UP  KEY_3_UP 
#define KEY_RETURN_UP KEY_1_UP

extern void widget_init (void);
extern INT8S widget_register (INT8U* widget_name, void (*widget_main) (void* pdata), void (*widget_destroy) (void));
extern INT8S widget_run (INT8U* widget_name, void* pdata);

extern void* widget_enter_sub (INT8U* widget_name, void* pdata);
extern INT8S widget_exit_sub (void* pdata);

extern void widget_lang_set (INT8U lang);
extern INT8U widget_lang_get (void);
#endif
