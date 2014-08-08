#ifndef LCD_H_H
#define LCD_H_H

extern void lcd_init (void);
extern void lcd_clear (void);

extern void lcd_write_dot (INT8U x, INT8U y, INT8U dat);
extern INT8U lcd_read_dot (INT8U x, INT8U y);

extern void lcd_write_byte (INT8U x, INT8U y, INT8U dat);
extern INT8U lcd_read_byte (INT8U x, INT8U y);

extern void lcd_h_line (INT8U x0, INT8U y0, INT8U x1, INT8U mode);
extern void lcd_v_line (INT8U x0, INT8U y0, INT8U y1, INT8U mode);

extern void lcd_light (INT8U on_off);

extern INT8U lcd_read_key (void);
extern void lcd_write_led (INT8U dat);

#endif
