#ifndef SYS_PARA_H_H
#define SYS_PARA_H_H

//片内FLASH最后2K字节作为系统参数存储区，每个参数占用32字节，可存储64个参数
#define SYS_PARA_1_NET_MAC              1
#define SYS_PARA_2_NET_IP               2
#define SYS_PARA_3_NET_MASK             3
#define SYS_PARA_4_NET_GATE             4      
#define SYS_PARA_5_NET_DNS1             5
#define SYS_PARA_6_NET_DNS2             6
#define SYS_PARA_7                      7
#define SYS_PARA_8                      8
#define SYS_PARA_9                      9
#define SYS_PARA_10                     10
#define SYS_PARA_11                     11
#define SYS_PARA_12                     12    
#define SYS_PARA_13                     13    
#define SYS_PARA_14                     14
#define SYS_PARA_15                     15
#define SYS_PARA_16                     16

#define SYS_PARA_17                     17
#define SYS_PARA_18                     18
#define SYS_PARA_19                     19
#define SYS_PARA_20                     20      
#define SYS_PARA_21                     21
#define SYS_PARA_22                     22
#define SYS_PARA_23                     23
#define SYS_PARA_24                     24
#define SYS_PARA_25                     25
#define SYS_PARA_26                     26
#define SYS_PARA_27                     27
#define SYS_PARA_28                     28    
#define SYS_PARA_29                     29    
#define SYS_PARA_30                     30
#define SYS_PARA_31                     31
#define SYS_PARA_32                     32

#define SYS_PARA_33                     33                    
#define SYS_PARA_34                     34
#define SYS_PARA_35                     35
#define SYS_PARA_36                     36    
#define SYS_PARA_37                     37
#define SYS_PARA_38                     38
#define SYS_PARA_39                     39
#define SYS_PARA_40                     40
#define SYS_PARA_41                     41
#define SYS_PARA_42                     42
#define SYS_PARA_43                     43
#define SYS_PARA_44                     44  
#define SYS_PARA_45                     45  
#define SYS_PARA_46                     46
#define SYS_PARA_47                     47
#define SYS_PARA_48                     48

#define SYS_PARA_49_RESET_CNT           49
#define SYS_PARA_50_SOFT_VER            50
#define SYS_PARA_51_HARD_VER            51
#define SYS_PARA_52                     52    
#define SYS_PARA_53                     53
#define SYS_PARA_54                     54
#define SYS_PARA_55                     55
#define SYS_PARA_56                     56
#define SYS_PARA_57                     57
#define SYS_PARA_58                     58
#define SYS_PARA_59                     59
#define SYS_PARA_60                     60  
#define SYS_PARA_61                     61  
#define SYS_PARA_62                     62
#define SYS_PARA_63                     63
#define SYS_PARA_64                     64

extern INT8U* sys_para_read (INT8U id);
extern INT8S sys_para_write (INT8U id, INT8U* buf, INT8U size); 
extern void sys_para_reset (void);

#endif
