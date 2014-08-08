#ifndef RG_LED_H_H
#define RG_LED_H_H

//红绿双色LED闪烁控制
//绿灯：运行灯，空闲时闪烁
//红灯：通信灯，通信时闪烁

extern void rg_led_init (void);
extern void r_led_flash (void);
extern void r_led (INT8U onoff);
extern void g_led (INT8U onoff);

#endif
