/*****************************************
 *  Copyright © 2001-2005
 *  Sigma Designs, Inc. All Rights Reserved
 *  Proprietary and Confidential
 ******************************************/
                                                                                
/*
 * Smart card related definitions, and function prototypes.
 */
#ifndef _SCARD_H_
#define _SCARD_H_
                                                                                
#ifdef __KERNEL__
#include <linux/kernel.h>
#include <linux/module.h>
#include <asm/ioctl.h>
#else
#include <sys/ioctl.h>
#endif /* __KERNEL__ */

/* ioctl commands for user level applications*/
#define SCARD_IOC_MAGIC		'S'
#define SCARD_IOC_WARMRESET	_IO(SCARD_IOC_MAGIC, 0)
#define SCARD_IOC_CLOCKSTOP	_IO(SCARD_IOC_MAGIC, 1)
#define SCARD_IOC_CLOCKSTART	_IO(SCARD_IOC_MAGIC, 2)
                                                                                
#endif /* _SCARD_H_ */

