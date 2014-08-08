#ifndef GRAPHIC_H_H
#define GRAPHIC_H_H

extern void draw_bitmap (INT8U x0, INT8U y0, INT8U width, INT8U length, const INT8U* data);
extern void draw_line (INT8U x0, INT8U y0, INT8U x1, INT8U y1, INT8U mode);
extern void draw_rectangle (INT8U x0, INT8U y0, INT8U x1, INT8U y1, INT8U mode);
extern void draw_rectangle_fill (INT8U x0, INT8U y0, INT8U x1, INT8U y1, INT8U mode);

#endif
