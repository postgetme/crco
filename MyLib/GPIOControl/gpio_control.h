#ifndef GPIO_CONTROL_H_H
#define GPIO_CONTROL_H_H

extern INT8S io_init (char* io, char* dir);
extern INT8U io_read (char* io);
extern void io_write (char* io, INT8U val);
extern void io_reverse (char* io, INT8U val);//io输出反向，val为第一次输出值

#endif
