#include "includes.h"

#define SYS_PARA_START_KB 254

static INT8U sector_buf[1024];

INT8U* sys_para_read (INT8U id)
{
    if (id > 64)
    {
        return NULL;    
    }
    
    return (INT8U*)(SYS_PARA_START_KB * 1024 + id * 32);
}

INT8S sys_para_write (INT8U id, INT8U* buf, INT8U size)
{
    if (id > 64)
    {
        return -1;    
    }
    
    memcpy (sector_buf, (INT8U*)(SYS_PARA_START_KB * 1024 + 1024 * (id / 32)), 1024);
    memcpy (sector_buf + id * 32, buf, size > 32 ? 32 : size);
    
    IntMasterDisable();     
    FlashErase (SYS_PARA_START_KB * 1024 + 1024 * (id / 32));
    FlashProgram ((unsigned long*)sector_buf, SYS_PARA_START_KB * 1024 + 1024 * (id / 32), 1024);
    IntMasterEnable();
    
    return 0;        
}

void sys_para_reset (void)
{
    INT8U para_buf[32];
    
    para_buf[0] = 0x12;
    para_buf[1] = 0x34;
    para_buf[2] = 0x56;
    para_buf[3] = 0x78;
    para_buf[4] = 0x9a;
    para_buf[5] = 0xbc;
    sys_para_write (SYS_PARA_1_NET_MAC, para_buf, 6); 
    
    para_buf[0] = 192;
    para_buf[1] = 168;
    para_buf[2] = 1;
    para_buf[3] = 168;
    sys_para_write (SYS_PARA_2_NET_IP, para_buf, 4);
    
    para_buf[0] = 255;
    para_buf[1] = 255;
    para_buf[2] = 0;
    para_buf[3] = 0;
    sys_para_write (SYS_PARA_3_NET_MASK, para_buf, 4);    
    
    para_buf[0] = 192;
    para_buf[1] = 168;
    para_buf[2] = 1;
    para_buf[3] = 1;
    sys_para_write (SYS_PARA_4_NET_GATE, para_buf, 4);

    para_buf[0] = 50;
    para_buf[1] = 60;
    para_buf[2] = 188;
    para_buf[3] = 178;
    sys_para_write (SYS_PARA_5_NET_DNS1, para_buf, 4);
    
    para_buf[0] = 50;
    para_buf[1] = 60;
    para_buf[2] = 188;
    para_buf[3] = 179;
    sys_para_write (SYS_PARA_6_NET_DNS2, para_buf, 4);
}
