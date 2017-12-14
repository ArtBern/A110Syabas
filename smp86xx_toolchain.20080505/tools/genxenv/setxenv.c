
/*
 * A small utility program to manipulate XENV keys.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#include "xenv.h"

#define MAX_XENV_SIZE  (256*1024)  /* 256KB */

static unsigned long hexstring2ulong(const char *str)
{
	unsigned long res = 0;
	for (; isxdigit(*str) && (*str != '\0'); str++) {
		res *= 16;
		res += isdigit(*str) ? ((*str) - '0') :
			(toupper(*str) - 'A' + 10);
	}
	return(res);
}

static unsigned long decstring2ulong(const char *str)
{
	unsigned long res = 0;
	for (; isdigit(*str) && (*str != '\0'); str++) {
		res *= 10;
		res += ((*str) - '0');
	}
	return(res);
}

static unsigned long string2ulong(const char *str)
{
	if ((*str == '0') && (toupper(*(str + 1)) == 'X'))
		return(hexstring2ulong(str + 2));
	else
		return(decstring2ulong(str));
}

static const char *basename(const char *str)
{
	const char *ptr;
	
	if ((str == NULL) || (strlen(str) == 0))
		return(str);

	ptr = str + strlen(str) - 1;
	while (ptr >= str) {
		if (*ptr == '/')
			break;
		else
			ptr--;
	}
	return(ptr + 1);
}

static int list_xenv(char *base,unsigned long size)
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
		int j;

		rec_attr=base[i]>>4;
		rec_size=((base[i]&0xf)<<8) + (((unsigned short)base[i+1])&0xff);
		recordname=base+i+2;
		key_len=strlen(recordname);
		x=(unsigned char *)(recordname+key_len+1);
		printf("(0x%02x) %5d %s ",rec_attr, rec_size-2-(key_len+1), recordname);

		for(j=0; j<rec_size-2-(key_len+1); j++)
			printf("%02x.", x[j] & 0xff);

		printf(" ");
		if ((rec_size-2-(key_len+1)) == sizeof(unsigned long)) {
			unsigned long v;
			memcpy(&v, x, sizeof(unsigned long));
			printf("0x%08lx", v);
		} else {
			for(j=0; j<rec_size-2-(key_len+1); j++)
				printf("%c", isprint(x[j]) ? x[j] : '.');
		}
		printf("\n");
		records++;
		i+=rec_size;
	}
	printf("%d records, %d bytes\n\n",records,env_size);
}

int main(int argc, char *argv[])
{
	int fno = 0, ret = 1, modified = 0, i;
	int binmode = 0, unset = 0;
	char *key = NULL, *value = NULL;
	char *fname = NULL;
	unsigned long *xenv_ptr, xenv_size;
	
	if (strcmp(basename(argv[0]), "unsetxenv") == 0)
		unset = 1;

	/* parse command line options */
	for (i = 1; i < argc; i++) {
		if ((unset == 0) && (strcmp(argv[i], "-b") == 0)) {
			binmode = 1;
		} else if ((unset == 0) && (strcmp(argv[i], "-u") == 0)) {
			unset = 1;
		} else if (strcmp(argv[i], "-f") == 0) {
			if ((i + 1) < argc) {
				fname = argv[i+1];
				i++;
			} else
				goto usage;
		} else if (strcmp(argv[i], "-k") == 0) {
			if ((i + 1) < argc) {
				key = argv[i+1];
				i++;
			} else
				goto usage;
		} else if ((unset == 0) && (strcmp(argv[i], "-v") == 0)) {
			if ((i + 1) < argc) {
				value = argv[i+1];
				i++;
			} else
				goto usage;
		} else
			goto usage;
	}

	/* check validity of command options */
	if (fname == NULL)
		goto usage;
	else if (!(((unset != 0) && (key != NULL) && (value == NULL) && (binmode == 0)) ||
			((unset == 0) && (key == NULL) && (value == NULL) && (binmode == 0)) ||
			((unset == 0) && (key != NULL) && (value == NULL)) ||
			((unset == 0) && (key != NULL) && (value != NULL))))
		goto usage;

	if ((fno = open(fname, O_RDONLY)) < 0) {
		printf("xenv_file (%s) open failure.\n", fname);
		goto bail_out;
	} else if ((xenv_ptr = (unsigned long *)malloc(MAX_XENV_SIZE)) == NULL) {
		printf("out of memory error\n");
		goto bail_out;
	} else if (read(fno, xenv_ptr, MAX_XENV_SIZE) <= 0) {
		printf("xenv_file (%s) read failure.\n", fname);
		goto bail_out;
	} else if (xenv_isvalid((char *)xenv_ptr, MAX_XENV_SIZE) < 0) {
		printf("invalid xenv_file (%s)\n", fname);
		goto bail_out;
	}

	close(fno);

	if (key == NULL) {
		/* List all keys */
		list_xenv((char *)xenv_ptr, MAX_XENV_SIZE);
	} else {
		if (unset != 0) {
			unsigned long sz = 0;
			value = xenv_get((char *)xenv_ptr, MAX_XENV_SIZE, key, &sz);
			if (value == NULL) {
				printf("key (%s) not found.\n", key);
				goto bail_out;
			}
			/* find the key, now delete it */
			xenv_set((char *)xenv_ptr, MAX_XENV_SIZE, key, NULL, 0, 0);
			modified = 1;
		} else {
			if (value != NULL) {
				if (binmode == 0)
					xenv_set((char *)xenv_ptr, MAX_XENV_SIZE, key, value, 0, strlen(value));
				else {
					unsigned long v = string2ulong(value);
					xenv_set((char *)xenv_ptr, MAX_XENV_SIZE, key, &v, 0, sizeof(unsigned long));
				}
				modified = 1;
			} else {
				/* List individual key */
				unsigned long sz = 0;
				char *val = NULL, *p;
				int j;

				val = xenv_get((char *)xenv_ptr, MAX_XENV_SIZE, key, &sz);
				if (val == NULL) {
					printf("key (%s) not found.\n", key);
					goto bail_out;
				} 
				printf("%d %s ", sz, key);
				for (p = val, j = 0; j < sz; j++, p++)
					printf("%02x.", *p & 0xff);
				printf(" ");
				if (sz == sizeof(unsigned long)) {
					unsigned long v;
					memcpy(&v, val, sizeof(unsigned long));
					printf("0x%08lx", v);
				} else {
					for (p = val, j = 0; j < sz; j++, p++) 
						printf("%c", isprint(*p) ? *p : '.');
				}
				printf("\n");
			}
		}
	}

	if (modified != 0) {
		xenv_size = xenv_isvalid((char *)xenv_ptr, MAX_XENV_SIZE);
		if ((fno = open(fname, O_WRONLY | O_SYNC)) < 0) {
			printf("xenv_file (%s) open failure.\n", fname);
			goto bail_out;
		} else if (write(fno, xenv_ptr, xenv_size) != xenv_size) {
			printf("xenv_file (%s) write failure.\n", fname);
			goto bail_out;
		}
	}

	ret = 0;

bail_out:
	if (xenv_ptr != NULL)
		free(xenv_ptr);
	if (fno >= 0)
		close(fno);
	return(ret);

usage:
	if (strcmp(basename(argv[0]), "unsetxenv") != 0) {
		printf("usage: %s -f xenv_file [[-u -k keyname]|[[-b] -k keyname [-v value]]]\n", argv[0]);
		printf("       -f xenv_file : to specify xenv file (e.g. /dev/mtdblock/0)\n");
		printf("       -k keyname : to specify xenv key\n");
		printf("       -u : unset/delete given xenv key\n");
		printf("       -v value : to specify value for given xenv key\n");
		printf("       -b : use value as binary value (i.e. unsigned long)\n");
		printf("   ex. %s -f xenv_file -- list all xenv keys\n", argv[0]);
		printf("       %s -f xenv_file -u -k key -- remove xenv key\n", argv[0]);
		printf("       %s -f xenv_file -k key -- get value from xenv key\n", argv[0]);
		printf("       %s -f xenv_file -b -k key -v val -- set binary value with xenv key\n", argv[0]);
	} else {
		printf("usage: %s -f xenv_file -k keyname\n", argv[0]);
		printf("       -f xenv_file : to specify xenv file (e.g. /dev/mtdblock/0)\n");
		printf("       -k keyname : to specify xenv key to be removed\n");
	}
	return(ret);
}

