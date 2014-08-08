#include "includes.h"

static INT16U mbm_send (INT8U* buf, INT16U size)
{
    
}

static INT16U mbm_recv (INT8U* buf, INT16U size)
{
    
}

static INT16U crc (INT16U* buf, INT16U size)
{
	INT16U crc = 0xffff, i, j, flag, a;
		
	for (i = 0; i < size; i++)
	{
		crc = crc ^ buf[i];
		for (j = 0; j < 8; j++)
		{
			a = crc;
			flag = a & 0x0001;
			crc = crc >> 1;
			if (flag == 1)
			{
				crc = crc ^ 0xa001;
			}
		}
	}
	
	buf[size + 1] = crc >> 8;
	buf[size] = crc & 255;
	
	return crc;    
}

INT8S mbm_init ()
{
    //timeout    
}

/*
return
 0 : OK
-1 : unknow modbus function
-2 : CRC error in the slave answer
-3 : timeout error
-4 : answer come from an other slave
-5 : revive length error
*/
INT8S mbm_send_recv (MBM_FRAME mbm_frame)
{
    INT8U buf[512];
    INT16U send_size = 0, recv_size = 0;
    INT16U mbm_read_len = 0;
    
    INT16U i = 0;
    
    switch (mbm_frame.function)
    {
        //read RW bits
        case 1:
            break;
            
        //read OR bits
        case 2:
            break;
        
        //read RW registers    
        case 3:
        //read OR registers    
        case 4:
            buf[0] = mbm_frame.address;
            buf[1] = mbm_frame.function;
            buf[2] = mbm_frame.start >> 8;
            buf[3] = mbm_frame.start & 0xff;
            buf[4] = mbm_frame.length >> 8;
            buf[5] = mbm_frame.length & 0xff;
            crc (buf, 6);
            send_size = 8;
            recv_size = 5 + mbm_frame.length * 2;            
            break;
        
        //write RW bit    
        case 5:
        //write RW register 
        case 6:
            buf[0] = mbm_frame.address;
            buf[1] = mbm_frame.function;
            buf[2] = mbm_frame.start >> 8;
            buf[3] = mbm_frame.start & 0xff;
            buf[4] = mbm_frame.buf[0] >> 8;
            buf[5] = mbm_frame.buf[1] & 0xff;
            crc (buf, 6);
            send_size = 8;
            recv_size = 8;               
            break;
        
        //read status
        case 7:
            buf[0] = mbm_frame.address;
            buf[1] = mbm_frame.function;            
            crc (buf, 2);
            send_size = 4;
            recv_size = 5; 
            break;
        
        //line test    
        case 8:
            buf[0] = mbm_frame.address;
            buf[1] = mbm_frame.function;
            buf[2] = 0;
            buf[3] = 0;
            buf[4] = 0;
            buf[5] = 0;
            crc (buf, 6);
            send_size = 8;             
            break;
        
        //write RW bits    
        case 15:
            break;
        
        //write RW registers    
        case 16:
            buf[0] = mbm_frame.address;
            buf[1] = mbm_frame.function;
            buf[2] = mbm_frame.start >> 8;
            buf[3] = mbm_frame.start & 0xff;
            buf[4] = mbm_frame.length >> 8;
            buf[5] = mbm_frame.length & 0xff;
            buf[6] = mbm_frame.length * 2;
            
            for (i = 0; i < mbm_frame.length; i++)
            {
				buf[7+i*2]=mbm_frame.buf[i] >> 8;
				buf[8+i*2]=mbm_frame.buf[i] & 0xff;                
            }
            
            crc (buf, 7 + mbm_frame.length * 2);
            send_size = mbm_frame.length * 2 + 9;
            recv_size = 8;           
            break;  
        
        default:
            return -1;       
    }
    
    for (i = 0; i < 3; i++)
    {
        mbm_write (buf, send_size);
        mbm_read_len = mbm_read (buf, sizeof (buf));
        
        if (mbm_read_len > 0)
        {
            break;           
        }
    }
    
    if (i == 3)
    {
        return -3;   
    }
        
    if (buf[0] != mbm_frame.address)
    {
        return -4;    
    }
    
    if (mbm_read_len != recv_size)
    {
        return -5;   
    }

    if (((buf[recv_size - 2] << 8) + buf[recv_size -1]) != crc (buf, recv_size - 2))
    {
        return -2;   
    }
    
    switch (mbm_frame.function)
    {
        //read RW bits
        case 1:
            break;
            
        //read OR bits
        case 2:
            break;
        
        //read RW registers    
        case 3:
        //read OR registers    
        case 4:
            for (i = 0; i < mbm_frame.length; i++)
            {
                mbm_frame.buf[i] = (buf[3 + (i * 2)] << 8) + buf[4 + i * 2];      
            }
            break;
        
        //write RW bit    
        case 5:
            break;
        //write RW register 
        case 6:
            break;
        
        //read status
        case 7:
            mbm_frame.buf[0] = buf[2];
            break;
        
        //line test    
        case 8:
            break;
        
        //write RW bits    
        case 15:
            break;
        
        //write RW registers    
        case 16:
            break;  
        
        default:
            return -1;       
    }
    
    return 0;  
}
