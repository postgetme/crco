#include "includes.h"

INT8S io_init (char* io, char* dir)
{
	INT32U pin = 0;
	INT32U periph = 0;
	INT32U base = 0;
	
	if (io == NULL)
	{
	 	return -1;
	}    
    
    if (strlen (io) == 3)   //"pa5"
    {
        if (io[0] == 'p' && (io[1] >= 'a' && io[1] <= 'h') && (io[2] >= '0' && io[2] <= '7'))
        {
            if (strcmp (dir, "in") == 0 || strcmp (dir, "out") == 0)
            {
                pin = 1 << (io[2] - '0');   
            }
            else
            {
                return -1;   
            }    
        }
        else
        {
            return -1;   
        }    
    }
    else if (strlen (io) == 2)   //"pa"
    {
        if (io[0] == 'p' && (io[1] >= 'a' && io[1] <= 'h'))
        {
            if (strcmp (dir, "in") == 0 || strcmp (dir, "out") == 0)
            {
                pin = 0xff;    
            }
            else
            {
                return -1;   
            }            
        }
        else
        {
            return -1;   
        }        
    }
    else
    {
        return -1;   
    }
    
    periph = (1 << (io[1] - 'a')) + 0x20000000;
    
	switch (io[1])
	{
		case 'a':
			base = GPIO_PORTA_BASE;
			break;

		case 'b':
			base = GPIO_PORTB_BASE;
			break;

		case 'c':
			base = GPIO_PORTC_BASE;
			break;

		case 'd':
			base = GPIO_PORTD_BASE;
			break;

		case 'e':
			base = GPIO_PORTE_BASE;
			break;

		case 'f':
			base = GPIO_PORTF_BASE;
			break;

		case 'g':
			base = GPIO_PORTG_BASE;
			break;

		case 'h':
			base = GPIO_PORTH_BASE;
			break;

		default:
			break;
	}
	
	if (strcmp (dir, "out") == 0)
	{
		SysCtlPeripheralEnable(periph);
		GPIOPadConfigSet(base, pin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
		GPIODirModeSet(base, pin, GPIO_DIR_MODE_OUT);
	}
	else 
	{
		SysCtlPeripheralEnable(periph);
		GPIOPadConfigSet(base, pin, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);
		GPIODirModeSet(base, pin, GPIO_DIR_MODE_IN);
	}
	
	return 0;	
}

INT8U io_read (char* io)
{
	INT32U pin = 0;
	INT32U base = 0;    

	if (io == NULL)
	{
	 	return 0;
	}
    
	switch (io[1])
	{
		case 'a':
			base = GPIO_PORTA_BASE;
			break;

		case 'b':
			base = GPIO_PORTB_BASE;
			break;

		case 'c':
			base = GPIO_PORTC_BASE;
			break;

		case 'd':
			base = GPIO_PORTD_BASE;
			break;

		case 'e':
			base = GPIO_PORTE_BASE;
			break;

		case 'f':
			base = GPIO_PORTF_BASE;
			break;

		case 'g':
			base = GPIO_PORTG_BASE;
			break;

		case 'h':
			base = GPIO_PORTH_BASE;
			break;

		default:
			break;
	}
	
    if (strlen (io) == 3)   //"pa5"
    {
        pin = 1 << (io[2] - '0');
        
        return GPIOPinRead(base, pin) == 0 ? 0:1;   
    }
    else if (strlen (io) == 2)   //"pa"
    {
        pin = 0xff; 
        
        return GPIOPinRead(base, pin);   
    }
	else
	{
	 	return 0;
	}	
}

void io_write (char* io, INT8U val)
{
	INT32U pin = 0;
	INT32U base = 0;
	
	if (io == NULL)
	{
	 	return;
	}	    
   
	switch (io[1])
	{
		case 'a':
			base = GPIO_PORTA_BASE;
			break;

		case 'b':
			base = GPIO_PORTB_BASE;
			break;

		case 'c':
			base = GPIO_PORTC_BASE;
			break;

		case 'd':
			base = GPIO_PORTD_BASE;
			break;

		case 'e':
			base = GPIO_PORTE_BASE;
			break;

		case 'f':
			base = GPIO_PORTF_BASE;
			break;

		case 'g':
			base = GPIO_PORTG_BASE;
			break;

		case 'h':
			base = GPIO_PORTH_BASE;
			break;

		default:
			break;
	}
	
    if (strlen (io) == 3)   //"pa5"
    {
        pin = 1 << (io[2] - '0');
        
        if ((val & 0x01) == 0)
        {
           GPIOPinWrite(base, pin, 0);     
        }
        else
        {
           GPIOPinWrite(base, pin, pin);        
        }   
    }
    else if (strlen (io) == 2)   //"pa"
    {
        pin = 0xff;
        
        GPIOPinWrite(base, pin, val);       
    }	
}

void io_reverse (char* io, INT8U val)
{
	static INT8U flag = 0;

	if (io == NULL)
	{
	 	return;
	}

	if (val > 0)
	{
	 	val = 1;
	}

	if (flag == 0)
	{
   		io_write (io, val);
		flag = 1;	
	}
	else
	{
	    io_write (io, ~val);
		flag = 0;
	}
}
