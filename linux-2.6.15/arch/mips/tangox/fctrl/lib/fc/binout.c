
/*****************************************
 *  Copyright © 2001-2004
 *  Sigma Designs, Inc. All Rights Reserved
 *  Proprietary and Confidential
 ******************************************/

/* Utility program to turn a binary file into a C header file */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

static const char *vname = "binout"; /* Default variable name */

int main(int argc, char *argv[])
{
	int cnt = 0, fno = 0;
	unsigned char c;

	if ((fno = open(argv[1], O_RDONLY)) < 0)
		return(-1);
	else if ((argc == 3) && (argv[2] != NULL))
		vname = argv[2];

	printf("static const char %s[] = {\n", vname);
	
	while(read(fno, &c, sizeof(unsigned char)) == sizeof(unsigned char)) {
		if ((cnt > 0) && ((cnt % 16) == 0))
			printf("\n");
		printf("0x%02x,", c);
		cnt++;
	}
	printf("};\n");

	close(fno);
	return(0);
}

