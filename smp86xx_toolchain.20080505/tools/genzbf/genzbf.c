
/*****************************************
 Copyright (c) 2002-2005
 Sigma Designs, Inc. All Rights Reserved
 Proprietary and Confidential
 *****************************************/

/*
 * Utility program to generate zboot header prepend file.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ctype.h>
#include <string.h>
#include <sys/stat.h>

/* Get the definition needed for the header */
#include "zboot.h"

/* Globals */
static const char zboot_fsig[4] = { 'F', 'N', 'I', 'B' }; /* "BINF" */
static struct zboot_fhdr fhdr;
static struct stat stat_buf;
static char *ifname = NULL, *ofname = NULL;
static int inf, outf;

/* Target always little endian, so if host is big endian, swap is needed */
static int swap = 0;	

/* Convert a hex ASCII string to unsigned long number */
static int string2hex(const char *str, unsigned long *lptr)
{
  /* Need to starts with "0x" or "0X" */
  if ((*str != '0') && (toupper(*(str + 1)) != 'X'))
    return(-1);
  else
    str += 2;
  
  *lptr = 0;
  for (; *str != '\0'; str++) {
    if (!isxdigit(*str)) 
      return(-1);
    else
      *lptr = (*lptr * 16) + (isdigit(*str) ? (*str - '0') : (toupper(*str) - 'A' + 10));
  }
  return(0);
}

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

static void swap_endian(void *src, unsigned size)
{
  unsigned char *c = (unsigned char *)src;
  unsigned char *e = c + size - 1;
  unsigned char tmp;

  if (size <= 1)
    return; /* No need for swapping */

  for (; c < e; c++, e--) {
    tmp = *c;
    *c = *e;
    *e = tmp;
  }
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

/* Copy file */
static int copyfile(int ofno, int ifno)
{
  unsigned char c;
  
  /* From the beginning of the source */
  if (lseek(ifno, 0, SEEK_SET) < 0)
    return(-1);
  
  for (; read(ifno, &c, sizeof(unsigned char)) > 0;) {
    if (write(ofno, &c, sizeof(unsigned char)) != sizeof(unsigned char))
       return(-1);
  }
  return(0);
}

int main(int argc, char *argv[])
{
  int i;
  char *ptr;
	
  memset(&fhdr, 0, sizeof(struct zboot_fhdr));
  memcpy(&fhdr.signature, zboot_fsig, 4);
  
  if (argc <= 1)
    goto usage;
  else
    swap = endian();
	
  /* Check and setup options */
  for (i = 1; i < argc; i++) {
    if (*(argv[i]) != '-') {
      ifname = argv[i];
    } else if (strncmp(argv[i], "-o", 2) == 0) {
      if (++i >= argc)
	goto usage;
      ofname = argv[i];
    } else if (strncmp(argv[i], "-l", 2) == 0) {
      if (++i >= argc)
	goto usage;
      if (string2hex(argv[i], &fhdr.loadaddr) != 0)
	goto usage;
    } else if (strncmp(argv[i], "-s", 2) == 0) {
      if (++i >= argc)
	goto usage;
      if (string2hex(argv[i], &fhdr.startaddr) != 0)
	goto usage;
    } else if (strncmp(argv[i], "-a", 2) == 0) {
      if (++i >= argc)
	goto usage;
      ptr = argv[i];
      for (; *ptr != '\0'; ptr++) {
	switch(*ptr) {
	case 'l':
	  fhdr.attributes |= ZBOOT_ATTR_LOAD;	
	  break;
	case 'e':
	  fhdr.attributes |= ZBOOT_ATTR_ENCRYPT;	
	  break;
	case 'z':
	  fhdr.attributes |= ZBOOT_ATTR_GZIP;	
	  break;
	case 'x':
	  fhdr.attributes |= ZBOOT_ATTR_EXEC;	
	  break;
	case 'f':
	  fhdr.attributes |= ZBOOT_ATTR_FINAL;	
	  break;
	default:
	  printf("wrong attribute: l, e, x, z, f\n");
	  goto error;
	}			
      }
    } else {
	printf("Unknown option (%s)\n", argv[i]);
	goto usage;
    }
  }

  fhdr.version = ZBOOT_VERSION;

  if ((ifname == NULL) || (ofname == NULL))
    goto usage;
  else if ((inf = open(ifname, O_RDONLY)) < 0) {
    printf("open %s failed.\n", ifname);
    goto error;
  } else if ((outf = open(ofname, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH)) < 0) {
    close(inf);
    printf("open %s failed.\n", ofname);
    goto error;
  } else if (fstat(inf, &stat_buf) < 0) {
    close(inf);
    printf("stat on %s failed.\n", ofname);
    goto error;
  } else if (checksum(inf, &fhdr.checksum) < 0) {
    close(inf);
    close(outf);
    printf("reading %s failed\n", ifname);
    goto error;
  } 

  fhdr.size = stat_buf.st_size;

  if (swap != 0) {
    swap_endian(&fhdr.signature, sizeof(fhdr.signature));
    swap_endian(&fhdr.loadaddr, sizeof(fhdr.loadaddr));
    swap_endian(&fhdr.startaddr, sizeof(fhdr.startaddr));
    swap_endian(&fhdr.attributes, sizeof(fhdr.attributes));
    swap_endian(&fhdr.checksum, sizeof(fhdr.checksum));
    swap_endian(&fhdr.version, sizeof(fhdr.version));
    swap_endian(&fhdr.size, sizeof(fhdr.size));
  }

  if (write(outf, &fhdr, sizeof(struct zboot_fhdr)) != 
	     sizeof(struct zboot_fhdr)) {
    close(inf);
    close(outf);
    printf("writing %s failed\n", ofname);
    goto error;
  } else if (copyfile(outf, inf) < 0) {
    close(inf);
    close(outf);
    printf("writing %s failed\n", ofname);
    goto error;
  }
  
  close(inf);
  close(outf);

  chmod(ofname, 0644);
  
  printf("zboot file header:\n");
  printf("\tHost: %s endian\n", (swap != 0) ? "big" : "little");
  printf("\tsignature: 0x%08lx\n", fhdr.signature);
  printf("\tloadaddr: 0x%08lx\n", fhdr.loadaddr);
  printf("\tstartaddr: 0x%08lx\n", fhdr.startaddr);
  printf("\tattribute: 0x%08lx\n", fhdr.attributes);
  printf("\tchecksum: 0x%08lx\n", fhdr.checksum);
  printf("\tversion: 0x%08lx\n", fhdr.version);
  printf("\tsize: 0x%08lx\n", fhdr.size);
  return(0);
  
usage:
  printf("usage: %s -l load_addr -s start_addr -a lexzf -o outfile infile\n", argv[0]);
  printf("       attribute: l=load, e=encrypt, x=execute, z=gzipped, f=final\n");

error:
  return(1);
}

