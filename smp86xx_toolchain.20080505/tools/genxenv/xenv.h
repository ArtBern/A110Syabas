/*****************************************
 Copyright © 2004-2005
 Sigma Designs, Inc. All Rights Reserved
 Proprietary and Confidential
 *****************************************/
/**
  @file   xenv.h
  @brief  

  The prototypes below act on a clear area respecting the xenv format.

  (De)ciphering the area, as well as committing the changes (writing the
  area to flash) are trivial add-ons on top of this API.

  The underlying implementation is not optimized for speed 
  (read and write in O(nrecords)). There is no fragmentation (data completely
  rearranged at each write).

  It is not advised to repeatedly act on flash stored data, for
  the device does not support unlimited read/write operations (refer to spec)

  Power loss when committing the changes cause loss of the stored data.
  This can be avoided by storing the same data twice to two different sectors.

  --------------------------------------------------------------------------
  Specification of a secure storage on serial flash of reboot-persistent data (xenv format)

  We describe a way to concatenate (at most 4KByte area (12bits limit)) variable length
  records identified by a string, the `key' (working much like Windows registry)

  Page, seen as a byte array, is 
  
  0          4                   24                                          env_size        4KB
  | env_size | SHA-1 of following | xenv_record0 | xenv_record1 | .. | xenv_recordn | xx xx .. |

  The SHA-1 extent is env_size-24.

  Description of a xenv_record (bytes):

  |4bits   12bits (2bytes)   | variable, NULL terminated | variable         |
  attr     total record size   record name (string)        record value

  attr = 
   XENV_ATTR_RW 
   XENV_ATTR_RO can be written once only but can be deleted	
   XENV_ATTR_OTP this record can be written once only and cannot be deleted
  --------------------------------------------------------------------------

  @author Emmanuel Michon
  @date   2005-05-17
*/

#ifndef __XENV_H__
#define __XENV_H__

#define XENV_ATTR_RW  15
#define XENV_ATTR_RO  14
#define XENV_ATTR_OTP 13

/**
   Initial xenv formatting to `empty'

   Passing the size is not necessary, however, minimum 24bytes must be accessible starting at base.

   @param base  
*/
void xenv_format(char *base);

/// 
/**
   Check for compliance with xenv format

   May be corrupted by: 
   - forgot to format
   - power loss during sflash write
   - intrusion

   @param base  
   @param size  
   @return -ReturnValue-: env_size>=0 if valid, -1 if not.
*/
int xenv_isvalid(char *base,unsigned long size);

#define XENV_OK 0
#define XENV_ERROR -1
#define XENV_INSUFFICIENT_SIZE -2
#define XENV_NOT_FOUND -3
#define XENV_INVALID_MODE -4

/**
   Look for `recordname' in the clear area pointed to by base,size
   
   Returns the adress or the record or NULL if no record or error.

   Example
   {
     unsigned char *data;
     unsigned long size;
     
     data=xenv_get(cleararea,PAGE_SIZE,"myhash",&size);
   }

   @param base  
   @param size  
   @param recordname    
   @param datasize: will be updated to actual size 
   @return -ReturnValue-
*/
char *xenv_get(char *base, unsigned long size, char *recordname, unsigned long *datasize);

/**
   Sets `recordname' in the clear area pointed to by base,size
   The area is rearranged to take the change into account (no fragmentation)
   
   The empty string is a valid key. 
   Storage of zero-length records is ok.
   Updating a record with a smaller or larger size is ok.
   
   Special: passing a NULL src means: delete the existing record. In this case, datasize is ignored.
   
   A record of attr XENV_ATTR_RW can be updated to a different attr. Others are immutable.
   
   It is advised to call the `commit' (write sector to flash) function if this
   call is successful.

   Returns 
   RM_INVALIDMODE if setting an existing XENV_ATTR_RO/_OTP for the second time, 
                  or deleting a XENV_ATTR_OTP record
   RM_INSUFFICIENT_SIZE if room is exhausted
   RM_ERROR if clear area is not compliant with xenv format (data uninitialized or corrupted).

   @param base  
   @param size  
   @param recordname    
   @param src
   @param attr  
   @param datasize
   @return -ReturnValue-
*/
int xenv_set(char *base, unsigned long size, char *recordname,void *src,unsigned char attr,unsigned long datasize);

int xenv_list(char *base,unsigned long size);

#endif // __XENV_H__
