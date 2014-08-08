//KEY_2.0 Designed by Mei_Lin at 20101224

/*
#!/bin/bash

echo "#ifndef KEY_MSG_DEF_H_H"
echo "#define KEY_MSG_DEF_H_H"
echo

for ((i = 1; i <= 24; i++))
do
			printf "#define KEY_%d_UP           (1<<24 | 1<<%d)\n" $i	$((i-1))
			printf "#define KEY_%d_DOWN         (2<<24 | 1<<%d)\n"  $i	$((i-1))
			printf "#define KEY_%d_DOUBLE       (3<<24 | 1<<%d)\n"  $i	$((i-1))
			printf "#define KEY_%d_PUSH         (4<<24 | 1<<%d)\n\n" $i	$((i-1))
done

echo "#endif"
*/

//消息格式: 32位(INT32U msg;) = 高8位(消息类型,按下,弹起,按住,双击) + 低24位(消息值,24个按键每个按键占1位)
//支持组合键, 可按需求添加
//#define KEY_1_2_3_4_PUSH    (4<<24 | 0x0F)
//#define KEY_2_9_DOUBLE      (3<<24 | 0x102)
//#define KEY_ALL_DOWN        (2<<24 | 0xFFFFFF)

#ifndef KEY_MSG_DEF_H_H
#define KEY_MSG_DEF_H_H

#define KEY_1_UP           (1<<24 | 1<<0)
#define KEY_1_DOWN         (2<<24 | 1<<0)
#define KEY_1_DOUBLE       (3<<24 | 1<<0)
#define KEY_1_PUSH         (4<<24 | 1<<0)

#define KEY_2_UP           (1<<24 | 1<<1)
#define KEY_2_DOWN         (2<<24 | 1<<1)
#define KEY_2_DOUBLE       (3<<24 | 1<<1)
#define KEY_2_PUSH         (4<<24 | 1<<1)

#define KEY_3_UP           (1<<24 | 1<<2)
#define KEY_3_DOWN         (2<<24 | 1<<2)
#define KEY_3_DOUBLE       (3<<24 | 1<<2)
#define KEY_3_PUSH         (4<<24 | 1<<2)

#define KEY_4_UP           (1<<24 | 1<<3)
#define KEY_4_DOWN         (2<<24 | 1<<3)
#define KEY_4_DOUBLE       (3<<24 | 1<<3)
#define KEY_4_PUSH         (4<<24 | 1<<3)

#define KEY_5_UP           (1<<24 | 1<<4)
#define KEY_5_DOWN         (2<<24 | 1<<4)
#define KEY_5_DOUBLE       (3<<24 | 1<<4)
#define KEY_5_PUSH         (4<<24 | 1<<4)

#define KEY_6_UP           (1<<24 | 1<<5)
#define KEY_6_DOWN         (2<<24 | 1<<5)
#define KEY_6_DOUBLE       (3<<24 | 1<<5)
#define KEY_6_PUSH         (4<<24 | 1<<5)

#define KEY_7_UP           (1<<24 | 1<<6)
#define KEY_7_DOWN         (2<<24 | 1<<6)
#define KEY_7_DOUBLE       (3<<24 | 1<<6)
#define KEY_7_PUSH         (4<<24 | 1<<6)

#define KEY_8_UP           (1<<24 | 1<<7)
#define KEY_8_DOWN         (2<<24 | 1<<7)
#define KEY_8_DOUBLE       (3<<24 | 1<<7)
#define KEY_8_PUSH         (4<<24 | 1<<7)

#define KEY_9_UP           (1<<24 | 1<<8)
#define KEY_9_DOWN         (2<<24 | 1<<8)
#define KEY_9_DOUBLE       (3<<24 | 1<<8)
#define KEY_9_PUSH         (4<<24 | 1<<8)

#define KEY_10_UP           (1<<24 | 1<<9)
#define KEY_10_DOWN         (2<<24 | 1<<9)
#define KEY_10_DOUBLE       (3<<24 | 1<<9)
#define KEY_10_PUSH         (4<<24 | 1<<9)

#define KEY_11_UP           (1<<24 | 1<<10)
#define KEY_11_DOWN         (2<<24 | 1<<10)
#define KEY_11_DOUBLE       (3<<24 | 1<<10)
#define KEY_11_PUSH         (4<<24 | 1<<10)

#define KEY_12_UP           (1<<24 | 1<<11)
#define KEY_12_DOWN         (2<<24 | 1<<11)
#define KEY_12_DOUBLE       (3<<24 | 1<<11)
#define KEY_12_PUSH         (4<<24 | 1<<11)

#define KEY_13_UP           (1<<24 | 1<<12)
#define KEY_13_DOWN         (2<<24 | 1<<12)
#define KEY_13_DOUBLE       (3<<24 | 1<<12)
#define KEY_13_PUSH         (4<<24 | 1<<12)

#define KEY_14_UP           (1<<24 | 1<<13)
#define KEY_14_DOWN         (2<<24 | 1<<13)
#define KEY_14_DOUBLE       (3<<24 | 1<<13)
#define KEY_14_PUSH         (4<<24 | 1<<13)

#define KEY_15_UP           (1<<24 | 1<<14)
#define KEY_15_DOWN         (2<<24 | 1<<14)
#define KEY_15_DOUBLE       (3<<24 | 1<<14)
#define KEY_15_PUSH         (4<<24 | 1<<14)

#define KEY_16_UP           (1<<24 | 1<<15)
#define KEY_16_DOWN         (2<<24 | 1<<15)
#define KEY_16_DOUBLE       (3<<24 | 1<<15)
#define KEY_16_PUSH         (4<<24 | 1<<15)

#define KEY_17_UP           (1<<24 | 1<<16)
#define KEY_17_DOWN         (2<<24 | 1<<16)
#define KEY_17_DOUBLE       (3<<24 | 1<<16)
#define KEY_17_PUSH         (4<<24 | 1<<16)

#define KEY_18_UP           (1<<24 | 1<<17)
#define KEY_18_DOWN         (2<<24 | 1<<17)
#define KEY_18_DOUBLE       (3<<24 | 1<<17)
#define KEY_18_PUSH         (4<<24 | 1<<17)

#define KEY_19_UP           (1<<24 | 1<<18)
#define KEY_19_DOWN         (2<<24 | 1<<18)
#define KEY_19_DOUBLE       (3<<24 | 1<<18)
#define KEY_19_PUSH         (4<<24 | 1<<18)

#define KEY_20_UP           (1<<24 | 1<<19)
#define KEY_20_DOWN         (2<<24 | 1<<19)
#define KEY_20_DOUBLE       (3<<24 | 1<<19)
#define KEY_20_PUSH         (4<<24 | 1<<19)

#define KEY_21_UP           (1<<24 | 1<<20)
#define KEY_21_DOWN         (2<<24 | 1<<20)
#define KEY_21_DOUBLE       (3<<24 | 1<<20)
#define KEY_21_PUSH         (4<<24 | 1<<20)

#define KEY_22_UP           (1<<24 | 1<<21)
#define KEY_22_DOWN         (2<<24 | 1<<21)
#define KEY_22_DOUBLE       (3<<24 | 1<<21)
#define KEY_22_PUSH         (4<<24 | 1<<21)

#define KEY_23_UP           (1<<24 | 1<<22)
#define KEY_23_DOWN         (2<<24 | 1<<22)
#define KEY_23_DOUBLE       (3<<24 | 1<<22)
#define KEY_23_PUSH         (4<<24 | 1<<22)

#define KEY_24_UP           (1<<24 | 1<<23)
#define KEY_24_DOWN         (2<<24 | 1<<23)
#define KEY_24_DOUBLE       (3<<24 | 1<<23)
#define KEY_24_PUSH         (4<<24 | 1<<23)

#endif
