/*----------------------------------------------------------------------------
 *      RL-ARM - FlashFS
 *----------------------------------------------------------------------------
 *      Name:    FILE_CONFIG.C
 *      Purpose: Configuration of RL FlashFS by user
 *      Rev.:    V4.12
 *----------------------------------------------------------------------------
 *      This code is part of the RealView Run-Time Library.
 *      Copyright (c) 2004-2010 KEIL - An ARM Company. All rights reserved.
 *---------------------------------------------------------------------------*/

#include <File_Config.h>

//-------- <<< Use Configuration Wizard in Context Menu >>> -----------------
//
// <e>Flash Drive
// ==============
// <i> Enable Embedded Flash Drive [F:]
#define FL_DEV      0

//   <o>Target device Base address <0x0-0xFFFFF000:0x1000>
//   <i> Define the target device Base address.
//   <i> Default: 0x80000000
#define FL_BADR     0x80000000

//   <o>Device Size in bytes <0x10000-0xFFFFF000:0x8000>
//   <i> Define the size of Flash device
//   <i> Default: 0x100000 (1MB)
#define FL_SIZE     0x0200000

//   <o>Initial Content of Erased Memory <0=> 0x00 <0xFF=> 0xFF
//   <i> Define the initial value for erased Flash data
//   <i> Default: 0xFF
#define FL_INITV    0xFF

//   <s.80>Device Description file
//   <i> Specify a file name with a relative path
//   <i> Default: FS_FlashDev.h
#define FL_HFILE    "Flash\FS_FlashDev.h"

// </e>
// <e>SPI Flash Drive
// ==================
// <i> Enable SPI Flash Drive [S:]
#define SF_DEV      0

//   <o>Initial Content of Erased Memory <0=> 0x00 <0xFF=> 0xFF
//   <i> Define the initial value for erased Flash data
//   <i> Default: 0xFF
#define SF_INITV    0xFF

//   <s.80>Device Description file
//   <i> Specify a file name with a relative path
//   <i> Default: FS_SPI_FlashDev.h
#define SF_HFILE    "FS_SPI_FlashDev.h"

// </e>
// <e>RAM Drive
// ============
// <i> Enable Embedded RAM Drive  [R:]
#define RAM_DEV     0

//   <o>Target device Base address <0x0-0xFFFFF000:0x1000>
//   <i> Define the target device Base address.
//   <i> Default: 0x81000000
#define RAM_BADR    0x81000000

//   <o>Device Size in bytes <0x4000-0xFFFFF000:0x4000>
//   <i> Define the size of RAM device
//   <i> Default: 0x40000
#define RAM_SIZE    0x040000

//   <o>Number of Sectors   <8=> 8  <16=> 16  <32=> 32 <64=> 64 <128=> 128
//   <i> Define number of virtual sectors for RAM device
//   <i> Default: 32
#define RAM_NSECT   8

// </e>
// <e>Memory Card Drive
// ====================
// <i> Enable Memory Card Drive  [M:]
#define MC_DEV      1

//   <o>File Data Cache  <0=> OFF  <2=> 1KB  <4=>  2KB <8=> 4KB 
//                       <16=> 8KB  <32=> 16KB  <64=> 32KB
//   <i> Define Data Cache buffer size for file IO. 
//   <i> Increase this number for faster r/w access.
//   <i> Default: 4 kB
#define MC_CSIZE    8

//   <e>Relocate Cache Buffer
//   <i> Locate Cache Buffer at a specific address.
//   <i> Some devices like NXP LPC23xx require a Cache buffer
//   <i> for DMA transfer located at specific address.
#define MC_RELOC    0

//   <o>Cache Buffer address <0x0000-0xFFFFFE00:0x200>
//   <i> Define the Cache buffer base address.
//   <i> For LPC23xx/24xx devices this is USB RAM
//   <i> starting at 0x7FD00000.
#define MC_CADR     0x7FD00000

//   </e>
// </e>
// <e>USB Flash Drive
// ====================
// <i> Enable USB Flash Drive  [U:]
#define USB_DEV     0

//   <o>File Data Cache  <0=> OFF  <2=> 1KB  <4=>  2KB <8=> 4KB 
//                       <16=> 8KB  <32=> 16KB  <64=> 32KB
//   <i> Define Data Cache buffer size for file IO. 
//   <i> Increase this number for faster r/w access.
//   <i> Default: 4 kB
#define USB_CSIZE   8

//   </e>
//   <o>Default Drive  <1=> Flash  <2=> SPI Flash  <3=> RAM  
//                     <4=> Memory Card <5=> USB Flash
//   <i> This drive is used when a Drive letter is not provided
#define DEF_DRIVE   4

//   <o>CPU Clock Frequency [Hz]<0-100000000>
//   <i> Define the CPU Clock frequency used for
//   <i> flash programming and erasing
#define CPU_CLK     60000000

//------------- <<< end of configuration section >>> -----------------------


#include <File_lib.c>

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
