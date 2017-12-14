/*****************************************
 Copyright © 2004-2005
 Sigma Designs, Inc. All Rights Reserved
 Proprietary and Confidential
 *****************************************/
#include <stdio.h>
#include <string.h>
#include "sha1sum.h"
#include "xenv.h"

/* xenv_lookup looks for a record
 *  return -1 if not found
 *  return -2 if parsing failed
 *  return offset of record if found
 */

static int xenv_lookup(char *base,unsigned long size,char *recordname)
{
	char *p;
	int i;
	int env_size;
	
	env_size=xenv_isvalid(base,size);
	
	if (env_size<0) 
		return -2;
	
	p=(char *)base;
	i=24; 			// jump over header
	
	while(i<env_size){
		unsigned short rec_size=((p[i]&0xf)<<8) + (((unsigned short)p[i+1])&0xff);

		if (!strcmp(recordname, p+i+2)) 
			return i;
		
		i+=rec_size;
	}
	
	return -1;
}

void xenv_format(char *base)
{
	unsigned int *ptr = (unsigned int *)base;
	ptr[0]=24;
	sha1sum((const unsigned char *)(ptr+6),0,(char *)(ptr+1));
}

int xenv_isvalid(char *base,unsigned long size)
{
	unsigned long env_size=((unsigned long *)base)[0];
	
	if ((24<=env_size)&&(env_size<=size)) {
		char hash[20];
		
		sha1sum((const unsigned char *)(base+24),env_size-24,hash);
		if (memcmp(base+4,hash,20)!=0) {
			fprintf(stderr,"ERROR: invalid xenv - wrong sha1 \n");
			return -1;
		}
	}
	else {
		fprintf(stderr,"ERROR: invalid xenv - invalid size %d\n",env_size);
		return -1;
	}

	return env_size;
}

char *xenv_get(char *base, unsigned long size, char *recordname, unsigned long *datasize)
{
	unsigned long *p = (unsigned long *)base;
	unsigned long data_len;
	unsigned long key_len;
	unsigned long env_size;
	
	int i;
	
	env_size=p[0];
	i = xenv_lookup(base, size, recordname);
	if(i<0) 
		return NULL;

	// else we found the record
	key_len=strlen(recordname);
	data_len=((base[i] & 0xf)<<8) | (((unsigned long)base[i+1])&0xff);
	data_len-=2+key_len+1;
	
	*datasize=data_len;
	return base+i+2+key_len+1;

}

int xenv_set(char *base, unsigned long size, char *recordname, void *src, unsigned char attr, unsigned long datasize)
{
	unsigned long *p;
	unsigned long env_size;
	unsigned long rec_attr;
	unsigned long rec_size;
	unsigned long key_len;
	int i;
#ifndef NDEBUG
	fprintf(stderr, "[%s], length %d\n", recordname, datasize);
#endif
	p=(unsigned long *)base;
	i = xenv_lookup(base, size, recordname);
	if(i==-2) 
		return XENV_ERROR;
	env_size=p[0];

	if(i>=0) {
#ifndef NDEBUG
		fprintf(stderr, "deleting record\n");
#endif	
		rec_attr=base[i]>>4;
		rec_size=((base[i]&0xf)<<8) + (((unsigned long)base[i+1])&0xff);
			
		if ((rec_attr==XENV_ATTR_RO)&&src) {
			fprintf(stderr, "ERROR: Cannot change ro record\n");
			return XENV_INVALID_MODE;
		}
		
		if (rec_attr==XENV_ATTR_OTP) {
			fprintf(stderr, "ERROR: Cannot change/clear otp record\n");
			return XENV_INVALID_MODE;
		}

		// delete the record. Use memmove because of overlapping
		memmove(base+i, base+i+rec_size, env_size-(i+rec_size));
		env_size-=rec_size;
	}
		
	// add the record at the end if needed.
	if(src) {
		i=env_size;
		key_len=strlen(recordname);
		rec_size=2+key_len+1+datasize;

		if((i+rec_size)>=size)
			return XENV_INSUFFICIENT_SIZE;
		
		base[i]=((attr&0xf)<<4) | ((rec_size>>8)&0xf);
		base[i+1]=rec_size&0xff;
		
		memcpy(base+i+2, recordname, key_len+1);
		memcpy(base+i+2+key_len+1, src, datasize);
		
		env_size+=rec_size;
	}
	
	p[0]=env_size;
	sha1sum((const unsigned char *)(base+24),env_size-24, base+4);

	return XENV_OK;
}

int xenv_list(char *base,unsigned long size)
{
	int i;
	int env_size;
	unsigned long records=0;

	env_size=xenv_isvalid(base,size);
	
	if (env_size<0) 
		return XENV_ERROR;
	
	i=24; 			// jump over header
	
	while(i<env_size){
		char rec_attr;
		unsigned short rec_size;
		char *recordname;
		unsigned long key_len;
		unsigned char *x;
#ifndef NDEBUG
		int j;
#endif
		rec_attr=base[i]>>4;
		rec_size=((base[i]&0xf)<<8) + (((unsigned short)base[i+1])&0xff);
		recordname=base+i+2;
		key_len=strlen(recordname);
		x=(unsigned char *)(recordname+key_len+1);
		fprintf(stderr, "(0x%02x) %5d %s ",rec_attr, rec_size-2-(key_len+1), recordname);
#ifndef NDEBUG
		for(j=0; j<rec_size-2-(key_len+1); j++)
			fprintf(stderr, "%02x.", x[j]);
#endif
		fprintf(stderr, "\n");
		records++;
		i+=rec_size;
	}
#ifndef NDEBUG
	fprintf(stderr, "%d records, %d bytes\n\n",records,env_size);
#endif
}

