/*****************************************
 Copyright © 2004-2005
 Sigma Designs, Inc. All Rights Reserved
 Proprietary and Confidential
 *****************************************/
/**
  @file   emhwlib_lram.h
  @brief  

  Map of the localram (8KBytes)

  Traditionnally the start of localram is used to setup
  a few kilobytes bootstrap routine code+data
  (cache init, tlb init, load something bigger to DRAM, jump there).

  Fixed offsets are defined in this file as communication devices
  between hardware blocks.
  Even debug locations must be present here.

  The bootstrap routine is expected to preserve these and setup
  its stack under LR_STACKTOP.

  Keep addresses increasing in this file.

  See emhwlib_resources_shared.h how some resources bw. 0 and 0x100 are used already
  only when uCLinux is up with irq handler running

  @author Emmanuel Michon
  @date   2005-03-17
*/

#ifndef __EMHWLIB_LRAM_H__
#define __EMHWLIB_LRAM_H__

#if (EM86XX_CHIP<EM86XX_CHIPID_TANGO3)

#define LR_CPU_IDLELOOP          0x00000000 /* CPU uses 0x80 bytes, up to 0x0080 */
#define LR_UCLINUX_END           0x00000100

#define LR_VSYNC_STRUCT          0x00000200 /* 2KB of data structures */
#define LR_VSYNC_CODE            0x00000a00 /* 2KB of code */
#define LR_VSYNC_END             0x00001200

#define LR_STACKTOP              0x000017F4 /* in case a bootstrap routine needs a stack in local ram. Use this boundary */

#define LR_PCI_INTERRUPT_ENABLE  0x000017F4
#define LR_HOST_INTERRUPT_STATUS 0x000017F8
#define LR_CPU_BRU_JUMP          0x000017FC /* `bootrom_ucos jump' (debug purpose) */

#define LR_MU_PROFILE_STATUS     0x00001800

#define LR_DRAM_DMA_SUSPEND               0x00001c8c
#define LR_SUSPEND_ACK_MPEG0              0x00001c90
#define LR_SUSPEND_ACK_MPEG1              0x00001c94
#define LR_SUSPEND_ACK_AUDIO0             0x00001c98
#define LR_SUSPEND_ACK_AUDIO1             0x00001c9c
#define LR_SUSPEND_ACK_DEMUX              0x00001ca0
#define LR_SUSPEND_ACK_IH                 0x00001ca4

#define LR_HB_IH                 0x00001ca8

#define LR_IH_LOG_FIFO           0x00001cac /* in some cases (splash screen) find the location of the log_fifo is not that easy. Read it here. */

#define LR_HB_HOST               0x00001cb0
#define LR_HB_CPU                0x00001cb4
#define LR_HB_MPEG0              0x00001cb8
#define LR_HB_MPEG1              0x00001cbc
#define LR_HB_AUDIO0             0x00001cc0
#define LR_HB_AUDIO1             0x00001cc4
#define LR_HB_DEMUX              0x00001cc8
#define LR_HB_XPU                0x00001ccc

#define LR_IDMA                  0x00001cd0 /* 16bytes. Obsoletizes LR_HMMAD */

#define LR_ETH_MAC_LO            0x00001ce0 /* Ethernet MAC addr low 4 bytes */
#define LR_ETH_MAC_HI            0x00001ce4 /* Ethernet MAC addr high bytes */
#define LR_HB_VSYNC              0x00001ce8
 
#define LR_SW_VAL_VSYNC_COUNT    0x00001cec /* this location is used to count captured VSYNC */
#define LR_SW_VAL_PIXEL_ADDR     0x00001cf0 /* this location is used to store a pixel address to write the frame count */

#define LR_HMMAD                 0x00001cf4
#define LR_KEY_ZONE              0x00001D00 /* 0x200 bytes, up to 0x1F00 */
#define LR_YAMON_DIGITS          0x00001F00
#define LR_XPU_DUMP              0x00001F00 /* 0x80 bytes, up to 0x1F80 */

#define LR_VSYNC_PERIOD          0x00001FA0 /* 0x20 bytes, up to 0x1FC0 */

#define LR_RANDOM_SEED           0x00001FC8 /* 0x08 bytes, up to 0x1FD0 */
#define LR_LOCAL_DEBUG_PROBE     0x00001FD0 /* 0x20 bytes, up to 0x1FF0 */

#define LR_XENV_LOCATION         0x00001FF0 /* Location of XENV, found by XOS */
#define LR_GNET_MAC              0x00001FF4
#define LR_ZBOOT_STAGE           0x00001FF8
#define LR_XPU_STAGE             0x00001FFC

#elif (EM86XX_CHIP==EM86XX_CHIPID_TANGO3)

/* Julien to clarify --- em07may31 */
#define LR_VSYNC_STRUCT          0x00000200 /* 2KB of data structures */
#define LR_VSYNC_CODE            0x00000a00 /* 2KB of code */
#define LR_VSYNC_END             0x00001200

#define LR_STACKTOP              0x000017f4 /* in case a bootstrap routine needs a stack in local ram. Use this boundary */

#define LR_PCI_INTERRUPT_ENABLE  0x000017f4
#define LR_HOST_INTERRUPT_STATUS 0x000017f8
#define LR_DRAM_DMA_SUSPEND      0x000017fc
#define LR_SUSPEND_ACK_MPEG0     0x00001800
#define LR_SUSPEND_ACK_MPEG1     0x00001804
#define LR_SUSPEND_ACK_AUDIO0    0x00001808
#define LR_SUSPEND_ACK_AUDIO1    0x0000180c
#define LR_SUSPEND_ACK_DEMUX     0x00001810
#define LR_SUSPEND_ACK_IH        0x00001814
#define LR_HB_IH                 0x00001818
#define LR_HB_HOST               0x0000181c
#define LR_HB_CPU                0x00001820
#define LR_HB_MPEG0              0x00001824
#define LR_HB_MPEG1              0x00001828
#define LR_HB_AUDIO0             0x0000182c
#define LR_HB_AUDIO1             0x00001830
#define LR_HB_DEMUX              0x00001834
#define LR_HB_XPU                0x00001838
#define LR_HB_VSYNC              0x0000183c
#define LR_SW_VAL_VSYNC_COUNT    0x00001840 /* this location is used to count captured VSYNC */
#define LR_SW_VAL_PIXEL_ADDR     0x00001844 /* this location is used to store a pixel address to write the frame count */

/* more to go in the 0x1848..0x18fc range, to be attributed similar way */

#define LR_XENV2_RW              0x00001900 /* up to 768 bytes */

#define LR_XENV2_RO              0x00001d00 /* up to the end, 768 bytes. This area is written by xpu, r.o. for others */

#else

#error what is this chip?

#endif

#endif // __EMHWLIB_LRAM_H__
