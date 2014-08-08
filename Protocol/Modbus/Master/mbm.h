//MeiLin_20120318

#ifndef MBM_H_H
#define MBM_H_H

typedef struct
{
   INT8U address; 										
   INT8U function; 										
   INT16U start;									
   INT8U length;
   INT16U buf[100];									
}MBM_FRAME;

int mbm_init ();
int mbm_send_recv (MBM_FRAME mbm_frame);

#endif
