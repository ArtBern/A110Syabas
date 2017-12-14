/*****************************************
 Copyright � 2004-2005
 Sigma Designs, Inc. All Rights Reserved
 Proprietary and Confidential
 *****************************************/
/**
  @file   emhwlib_dram_others.h
  @brief  

  Addresses appear in increasing order. It is assumed
  that computing FM_IRQHANDLER_STACKTOP_USR-FM_IRQHANDLER_CODE
  is a proper way to access the max usable size for
  FM_IRQHANDLER_CODE.

  @author Emmanuel Michon
  @date   2005-04-11
*/

#ifndef __EMHWLIB_DRAM_OTHERS_H__
#define __EMHWLIB_DRAM_OTHERS_H__

#define FM_MEMCFG                  0x00000fc0
#define FM_IRQHANDLER_API          0x00001000
#define FM_IRQHANDLER_CODE         0x00011000
#define FM_IRQHANDLER_STACKTOP_USR 0x00040000 /* defined, but never used */
#define FM_IRQHANDLER_STACKTOP_IRQ 0x00048000
#define FM_IRQHANDLER_STACKTOP_FIQ 0x00050000
#define FM_STACKTOP_SVC            0x00058000
#define FM_DRM			   0x00058000
#define FM_GNET			   0x00058000 /* incompatible with DRM */
#define FM_BOOTLOADER_CODE         0x00060000
#define FM_RESERVED                0x00080000 /* The size reserved */

#endif // __EMHWLIB_DRAM_OTHERS_H__
