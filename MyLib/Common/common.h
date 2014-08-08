#ifndef COMMON_H_H
#define COMMON_H_H

extern INT8S big_small_mode_switch (void* p_data, INT8U data_len);
extern INT8U check_sum (INT8U* p_data, INT16U data_len);
extern int asc_to_hex(char* ascii, char* hex);
extern int hex_to_asc(char* hex, char* ascii);

#endif
