#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "xenv.h"
#include "sha1sum.h"

#define MAX_XENV_SIZE   (64*1024)
#define MAX_VALUE_SIZE  (4*1024)

static char *xenv=NULL;
static unsigned long xenvsize=0;

void usage(char *message)
{

	fprintf(stderr, message);
	fprintf(stderr, "Usage: genxenv <cmd> <xenvfile> [<key>] [<filename>]\n"
	       "<cmd>     : one of the following\n"
	       "  l : list key/value pairs\n"
	       "  g : get value for key\n"
	       "  s : set key/value pair\n"
	       "  d : delete key\n"
	       "  f : format\n"
	       "  c : check\n"
	       "<key>     : xenv key (string)\n"
	       "<filename>: file to use to read/write values, default\n"
	       "            to stdin and stdout\n"
	       "\n");
	
	exit(-1);
}

int load_value(const char *filename, char *buffer)
{
	int size;
	FILE *fd;

	if(filename)
		fd=fopen(filename, "r");
	else {
		fd=stdin;
#ifndef NDEBUG
		fprintf(stderr, "Reading value from stdin\n");
#endif
	}
	
	if(fd) {
		size=fread(buffer, 1, MAX_VALUE_SIZE, fd);
		if(filename) fclose(fd);
	}
	else
		size=-1;
	      
	if(size==-1) {
		perror("ERROR: load value from file");
		exit(-1);
	}
#ifndef NDEBUG
	fprintf(stderr, "Read value file, size = %d\n", size);
#endif
	return size;
}
	
int save_value(const char *filename, char *buffer, int size)
{
	FILE *fd;

	if(filename)
		fd=fopen(filename, "w");
	else
		fd=stdout;
	
	if(fd) {
		size=fwrite(buffer, 1, size, fd);
		if(filename) fclose(fd);
	}
	else
		size=-1;
	      
	if(size==-1) {
		perror("ERROR: saving value to file");
		exit(-1);
	}
#ifndef NDEBUG
	fprintf(stderr, "Wrote value file, size = %d\n", size);
#endif
	return size;
}
	
char *load_file(const char *filename)
{
	char *xenv;
	FILE *infile;
	
	xenv=malloc(MAX_XENV_SIZE);

	if(xenv==NULL) {
		perror("ERROR: cannot allocate xenv buffer");
		exit(-1);
	}

	infile=fopen(filename, "r");
	if(infile) {
		fread(xenv, sizeof(unsigned long), 1, infile);
		xenvsize=*((unsigned long *)xenv);
		fread(xenv+sizeof(unsigned long), 1, xenvsize-sizeof(unsigned long), infile);
		fclose(infile);
	}
	else
		xenvsize=0;
	      
	if((xenvsize==-1)||(xenvsize>MAX_XENV_SIZE)) {
		perror("ERROR: Could not load xenv from infile"); 
		exit(-1);
	}
#ifndef NDEBUG
	fprintf(stderr, "Loaded xenv file, xenvsize = %d\n", xenvsize);
#endif
	return xenv;
}

int save_file(const char *filename, char *xenv)
{
	FILE *outfile;
	int sz;
	
	outfile=fopen(filename, "w");
	if(outfile) {
		xenvsize=*(unsigned long *)xenv;
#ifndef NDEBUG
		fprintf(stderr, "Saving xenv file, xenvsize = %d\n", xenvsize);
#endif
        	sz=fwrite(xenv, 1, xenvsize, outfile);
		fclose(outfile);
	} else
		sz==-1;

	if(sz==-1) {
		perror("ERROR: saving value to file");
		exit(-1);
	}
		
	return sz;
}

int xenv_check_main(int argc, char *argv[])
{
	if(xenv_isvalid(xenv, MAX_XENV_SIZE)) {
#ifndef NDEBUG
		fprintf(stderr, "valid xenv, size=%d\n", xenvsize);
#endif
		return 0;
	} else {
		fprintf(stderr, "ERROR: invalid xenv\n");
		return -1;
	}
	
}

int xenv_delete_main(int argc, char *argv[])
{
	char *key;
	
	if(argc<3) usage("invalid delete command line");
	key = argv[2];
	return xenv_set(xenv, MAX_XENV_SIZE, key, NULL, 0, 0);
}


int xenv_format_main(int argc, char *argv[])
{
	xenv_format(xenv);
	return 0;
}

int xenv_get_main(int argc, char *argv[])
{
	char *key;
	char *value;
	unsigned long sz;
#ifndef NDEBUG
	int j;
#endif
	
	if(argc<3) usage("invalid get command line");
	key = argv[2];
	value = xenv_get(xenv, MAX_XENV_SIZE, key, &sz);

	if (value == NULL) {
		fprintf(stderr, "ERROR: no such key %s.\n", key);
		return -1;
	}
#ifndef NDEBUG
	fprintf(stderr, "%5d %s ", sz, key);
	for(j=0; j<sz; j++)
		fprintf(stderr, "%02x.", value[j]);
	fprintf(stderr, "\n");
#endif
	return save_value(argv[3], value, (int)sz);
}


int xenv_list_main(int argc, char *argv[])
{
	xenv_list(xenv, MAX_XENV_SIZE);
	return 0;
}

int xenv_set_main(int argc, char *argv[])
{
	char *key;
	char value[MAX_XENV_SIZE];
	int sz;
	
	if(argc<3) usage("invalid set command line");
	key = argv[2];
	sz=load_value(argv[3], value);
	return xenv_set(xenv, MAX_XENV_SIZE, key, value, 0, sz);
}


int main(int argc, char *argv[])
{

	int rc = -1;
	int changed = 0;
	
	if(argc<2) usage("invalid command line\n");

	xenv=load_file(argv[2]);

	switch(argv[1][0])
	{
		case 'c': // check
			rc = xenv_check_main(argc-1, argv+1);
			break;
		case 'd': // delete
			rc = xenv_delete_main(argc-1, argv+1);
			changed = 1;
			break;
		case 'f': // format
			rc = xenv_format_main(argc-1, argv+1);
			changed = 1;
			break;
		case 'g': // get/read
			rc = xenv_get_main(argc-1, argv+1);
			break;
		case 'l': // list/dump
			rc = xenv_list_main(argc-1, argv+1);
			break;
		case 's': // set/write
			rc = xenv_set_main(argc-1, argv+1);
			changed = 1;
			break;
		default:
			usage("unknown command\n");
			break;
	}
	
	if (changed)
		save_file(argv[2], xenv);

	if (xenv!=NULL)
		free(xenv);

	return rc;	
}
