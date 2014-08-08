//KEY_2.0 Designed by Mei_Lin at 20101224

#include "includes.h"

void key_phy_init (void)
{
	io_init ("pd7", "in");
}

INT32U key_phy_read (void)
{
	return io_read ("pd7");
}
