
/*****************************************
 Copyright (c) 2002-2005
 Sigma Designs, Inc. All Rights Reserved
 Proprietary and Confidential
 *****************************************/

/*
 * Utility program to generate simple checksum of given file
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>

static char *ifname = NULL;
static int inf = -1;

/* Target always little endian, so if host is big endian, swap is needed */
static int swap = 0;	

static inline unsigned long get_dword_from_buf(unsigned char *buf)
{
  unsigned long val = 0;
  if (swap != 0) 
    val = buf[3] | (buf[2] << 8) | (buf[1] << 16) | (buf[0] << 24); 
  else
    val = buf[0] | (buf[1] << 8) | (buf[2] << 16) | (buf[3] << 24); 
  return val;
}

/* Check the endian-ness of the host: 0=little, 1=big */
static int endian(void)
{
  union {
    unsigned long l;
    unsigned char c[sizeof(unsigned long)];
  } u;

  u.l = 0x12345678;
  return((u.c[0] == 0x12) ? 1 : 0);
}

static int checksum(int fno, unsigned long *lptr)
{
  unsigned long sum = 0;
  unsigned int i, size;
  unsigned char *ptr;
  unsigned long l;
  
  if (lseek(fno, 0, SEEK_SET) < 0)
    return(-1);
  
  for (; (size = read(fno, &l, sizeof(unsigned long))) == sizeof(unsigned long);) 
    sum += get_dword_from_buf((unsigned char *)&l);

  if (size & 0x3) {
    union {
      unsigned char c[4];
      unsigned long l;
    } buf;

    buf.l = 0;
    for (i = 0, ptr = (unsigned char *)&l; i < (size & 0x3); i++) 
      buf.c[i] = *ptr++;
    sum += get_dword_from_buf(&buf.c[0]);
  }	    
  *lptr = sum;
  return(0);
}

int main(int argc, char *argv[])
{
  int i;
  unsigned long csum = 0;

  if (argc < 2)
    goto usage;

  swap = endian();

  for (i = 1; i < argc; i++) {
    ifname = argv[i];
    if ((inf = open(ifname, O_RDONLY)) < 0) {
      printf("open %s failed.\n", ifname);
      goto error;
    } else if (checksum(inf, &csum) < 0) {
      close(inf);
      goto error;
    } 
    printf("%s\t0x%08lx\n", ifname, csum);
    close(inf);
  }
  return(0);
  
usage:
  printf("usage: %s file1 [file2 [file3 ..]]\n", argv[0]);

error:
  if (inf >= 0)
	close(inf);

  return(1);
}

