#include "includes.h"

static INT8U g_width;
static INT8U g_cnt;
static INT8U g_pos = 0;
static INT8U g_adj;

void bar_init (void)
{
	draw_rectangle_fill (121, 0, 127, 63, 0);
	
	draw_line (127, 3, 121, 3, 1);
	draw_line (126, 2, 122, 2, 1);
	draw_line (125, 1, 123, 1, 1);
	draw_line (124, 0, 124, 0, 1);

	draw_line (127, 60, 121, 60, 1);
	draw_line (126, 61, 122, 61, 1);
	draw_line (125, 62, 123, 62, 1);
	draw_line (124, 63, 124, 63, 1);
}

void bar_clear (void)
{
 	draw_rectangle_fill (122, 5, 126, 58, 0);
}

void bar_width_set (INT8U cnt)
{
	g_width = 54 / cnt; //注意,cnt大于54有问题???要改一下!!!
	g_cnt = cnt;
	g_adj = 54 % cnt;

	g_pos = 0;

	bar_clear ();
	draw_rectangle_fill (122, 5, 126, 4 + g_width, 1);	 	
}

void bar_pos (INT8U pos)
{
	INT8U i;

	bar_clear ();
	
	if (pos == g_cnt - 1)
	{
		draw_rectangle_fill (122, 59 - g_width, 126, 58, 1);	
	}
	else
	{
		if (pos < g_cnt - g_adj)
		{
			draw_rectangle_fill (122, 5 + g_width * pos, 126, 4 + g_width + g_width * pos, 1);
		}
		else
		{
			i = pos - (g_cnt - g_adj);
			draw_rectangle_fill (122, 5 + g_width * pos + i, 126, 4 + g_width + g_width * pos + i, 1);
		}
	}
}

void bar_up (void)
{	
	if (g_pos == 0)
	{
		g_pos = g_cnt - 1;
		bar_pos (g_pos);
	}
	else
	{
		bar_pos (--g_pos);
	}
}

void bar_down (void)
{
	if (g_pos == g_cnt - 1)
	{
		g_pos = 0;
		bar_pos (g_pos);
	}
	else
	{
		bar_pos (++g_pos);
	}
}
