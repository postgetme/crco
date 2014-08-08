#include "includes.h"

void app_task1(void* pdat)
{
	widget_register ("menu", menu_main, 0);
	widget_register ("screen_protect", screen_protect_main, 0);
	widget_register ("desktop", desktop_main, desktop_destroy);
	widget_register ("edit", edit_main, 0);
	widget_register ("time_set", time_set_main, 0);
	widget_register ("list", list_main, 0);

	widget_run ("desktop", (void*)0);	
	
	while (1)
	{
		OSTimeDly (10000);
	}
}
