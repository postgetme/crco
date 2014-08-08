#ifndef PCF8563_H_H
#define PCF8563_H_H

typedef struct
{
	INT16U year;
	INT8U month;
	INT8U day;
	INT8U hour;
	INT8U minute;
	INT8U second;
	INT8U week;
}TIME;

extern INT8S pcf8563_init (void);
extern INT8S pcf8563_read (TIME* ptime);
extern INT8S pcf8563_write (TIME* ptime);

#endif
