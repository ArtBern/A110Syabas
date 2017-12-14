#ifndef __FIP_USER_H__
#define __FIP_USER_H__

#define FIP_DISPLAY_MODE		0xa
#define MAX_FIP_RAM				23
#define NUM_SYMBOLS				8
#define NUM_CHARACTERS			72
#define NUM_DIGITS				7

/* Valid symbols */
#define AVC_FIP_ON				0
#define NTSC_FIP_ON  		    1
#define WMT_FIP_ON  			2
#define PAL_FIP_ON  			3
#define MPEG4_FIP_ON  		    4
#define HD_FIP_ON			    5
#define DVD_FIP_ON 			    6
#define DVI_FIP_ON              7
#define COLON_MIN_SEC_FIP_ON	-1
#define COLON_HOUR_MIN_FIP_ON	-1

/* FIP keys */
#define FIP_KEY_EJECT			0x00000001
#define FIP_KEY_NEXT			0x00000010
#define FIP_KEY_PREV			0x00000100
#define FIP_KEY_STOP			0x00001000
#define FIP_KEY_PLAYPAUSE		0x00010000
#define FIP_KEY_POWER			0x00100000

#endif
