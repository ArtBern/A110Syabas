#ifndef __FIP_MAP_H__
#define __FIP_MAP_H__

#define FIP_DIVIDER				27	/* default value */

/*
  14 SEGMENT LCD (EXTENDED CHARACTERS MAP)

     a 
   -----
f |\j| /| b 
  |i\|/k|
  g-- --h
e |n/|\l| c
  |/m| \|
   -----
     d   
*/
#define DIGIT_L(g,h,c,l,m,n,e,d)	((g << 7) | ( h <<6) | (c << 5) | (l << 4) | (m << 3) | (n << 2) | (e << 1) | d)
#define DIGIT_H(b7,b6,i,j,k,a,b,f)	((b7 << 7) | (b6 << 6) | (a << 5) | (f << 4) | (j << 3) | (k << 2) | (b << 1) | i)

/* sequence must match fipcharacters */
static const char fipcharactersmap[NUM_CHARACTERS+1] = " +-/0123456789<>ABCDEFGHIJKLMNOPQRSTUVWXYZ\\-abcdefghijklmnopqrstuvwxyz|_";

/* we use the inverted mask for clearing a digit without clearing other things */
static const char fipcharactermask[2] = {
	DIGIT_L(0,0,0,0,0,0,0,0), 
	DIGIT_H(1,1,0,0,0,0,0,0)
};

/* the format is lower byte, higher byte */
static const char fipcharacters[NUM_CHARACTERS][2] = {
	{DIGIT_L(0,0,0,0,0,0,0,0), DIGIT_H(0,0,0,0,0,0,0,0)},	//
	{DIGIT_L(1,1,0,0,1,0,0,0), DIGIT_H(0,0,0,1,0,0,0,0)},	// +
	{DIGIT_L(1,1,0,0,0,0,0,0), DIGIT_H(0,0,0,0,0,0,0,0)},	// -
	{DIGIT_L(0,0,0,0,0,1,0,0), DIGIT_H(0,0,0,0,1,0,0,0)},	// /
                                                       
	{DIGIT_L(0,0,1,0,0,0,1,1), DIGIT_H(0,0,0,0,0,1,1,1)},	// 0
	{DIGIT_L(0,0,0,0,1,0,0,0), DIGIT_H(0,0,0,1,0,0,0,0)},	// 1
	{DIGIT_L(1,1,0,0,0,0,1,1), DIGIT_H(0,0,0,0,0,1,1,0)},	// 2
	{DIGIT_L(1,1,1,0,0,0,0,1), DIGIT_H(0,0,0,0,0,1,1,0)},	// 3
	{DIGIT_L(1,1,1,0,0,0,0,0), DIGIT_H(0,0,0,0,0,0,1,1)},	// 4
	{DIGIT_L(1,1,1,0,0,0,0,1), DIGIT_H(0,0,0,0,0,1,0,1)},	// 5
	{DIGIT_L(1,1,1,0,0,0,1,1), DIGIT_H(0,0,0,0,0,1,0,1)},	// 6
	{DIGIT_L(0,0,1,0,0,0,0,0), DIGIT_H(0,0,0,0,0,1,1,0)},	// 7
	{DIGIT_L(1,1,1,0,0,0,1,1), DIGIT_H(0,0,0,0,0,1,1,1)},	// 8
	{DIGIT_L(1,1,1,0,0,0,0,1), DIGIT_H(0,0,0,0,0,1,1,1)},	// 9
                                                       
	{DIGIT_L(0,0,0,1,0,0,0,0), DIGIT_H(0,0,0,0,1,0,0,0)},	// <
	{DIGIT_L(0,0,0,0,0,1,0,0), DIGIT_H(0,0,1,0,0,0,0,0)},	// >
                                                       
	{DIGIT_L(1,1,1,0,0,0,1,0), DIGIT_H(0,0,0,0,0,1,1,1)},	// A
	{DIGIT_L(0,1,1,0,1,0,0,1), DIGIT_H(0,0,0,1,0,1,1,0)},	// B
	{DIGIT_L(0,0,0,0,0,0,1,1), DIGIT_H(0,0,0,0,0,1,0,1)},	// C
	{DIGIT_L(0,0,1,0,1,0,0,1), DIGIT_H(0,0,0,1,0,1,1,0)},	// D
	{DIGIT_L(1,1,0,0,0,0,1,1), DIGIT_H(0,0,0,0,0,1,0,1)},	// E
	{DIGIT_L(1,1,0,0,0,0,1,0), DIGIT_H(0,0,0,0,0,1,0,1)},	// F
	{DIGIT_L(0,1,1,0,0,0,1,1), DIGIT_H(0,0,0,0,0,1,0,1)},	// G
	{DIGIT_L(1,1,1,0,0,0,1,0), DIGIT_H(0,0,0,0,0,0,1,1)},	// H
	{DIGIT_L(0,0,0,0,1,0,0,1), DIGIT_H(0,0,0,1,0,1,0,0)},	// I
	{DIGIT_L(0,0,1,0,0,0,0,1), DIGIT_H(0,0,0,0,0,0,1,0)},	// J
	{DIGIT_L(1,0,0,1,0,0,1,0), DIGIT_H(0,0,0,0,1,0,0,1)},	// K
	{DIGIT_L(0,0,0,0,0,0,1,1), DIGIT_H(0,0,0,0,0,0,0,1)},	// L
	{DIGIT_L(0,0,1,0,0,0,1,0), DIGIT_H(0,0,1,0,1,0,1,1)},	// M
	{DIGIT_L(0,0,1,1,0,0,1,0), DIGIT_H(0,0,1,0,0,0,1,1)},	// N
	{DIGIT_L(0,0,1,0,0,0,1,1), DIGIT_H(0,0,0,0,0,1,1,1)},	// O
	{DIGIT_L(1,1,0,0,0,0,1,0), DIGIT_H(0,0,0,0,0,1,1,1)},	// P
	{DIGIT_L(0,0,1,1,0,0,1,1), DIGIT_H(0,0,0,0,0,1,1,1)},	// Q
	{DIGIT_L(1,1,0,1,0,0,1,0), DIGIT_H(0,0,0,0,0,1,1,1)},	// R
	{DIGIT_L(1,1,1,0,0,0,0,1), DIGIT_H(0,0,0,0,0,1,0,1)},	// S
	{DIGIT_L(0,0,0,0,1,0,0,0), DIGIT_H(0,0,0,1,0,1,0,0)},	// T
	{DIGIT_L(0,0,1,0,0,0,1,1), DIGIT_H(0,0,0,0,0,0,1,1)},	// U
	{DIGIT_L(0,0,1,1,0,0,0,0), DIGIT_H(0,0,1,0,0,0,1,0)},	// V
	{DIGIT_L(0,0,1,1,0,1,1,0), DIGIT_H(0,0,0,0,0,0,1,1)},	// W
	{DIGIT_L(0,0,0,1,0,1,0,0), DIGIT_H(0,0,1,0,1,0,0,0)},	// X
	{DIGIT_L(0,0,0,0,1,0,0,0), DIGIT_H(0,0,1,0,1,0,0,0)},	// Y
	{DIGIT_L(0,0,0,0,0,1,0,1), DIGIT_H(0,0,0,0,1,1,0,0)},	// Z
                                                       
	{DIGIT_L(0,0,0,1,0,0,0,0), DIGIT_H(0,0,1,0,0,0,0,0)},	// Slash
	{DIGIT_L(1,1,0,0,0,0,0,0), DIGIT_H(0,0,0,0,0,0,0,0)},	// -
                                                       
	{DIGIT_L(1,1,1,0,0,0,1,1), DIGIT_H(0,0,0,0,0,0,0,0)},	// a
	{DIGIT_L(1,1,1,0,0,0,1,1), DIGIT_H(0,0,0,0,0,0,0,1)},	// b
	{DIGIT_L(1,1,0,0,0,0,1,1), DIGIT_H(0,0,0,0,0,0,0,0)},	// c
	{DIGIT_L(1,1,1,0,0,0,1,1), DIGIT_H(0,0,0,0,0,0,1,0)},	// d
	{DIGIT_L(1,1,0,0,0,0,1,1), DIGIT_H(0,0,0,0,0,1,1,1)},	// e
	{DIGIT_L(1,0,0,0,0,0,1,0), DIGIT_H(0,0,0,0,0,1,0,1)},	// f
	{DIGIT_L(1,1,1,0,0,0,0,1), DIGIT_H(0,0,0,0,0,1,1,1)},	// g
	{DIGIT_L(1,1,1,0,0,0,1,0), DIGIT_H(0,0,0,0,0,0,0,1)},	// h
	{DIGIT_L(0,0,0,0,1,0,0,0), DIGIT_H(0,0,0,0,0,0,0,0)},	// i
	{DIGIT_L(0,0,1,0,0,0,0,1), DIGIT_H(0,0,0,0,0,0,0,0)},	// j
	{DIGIT_L(0,0,0,1,1,0,0,0), DIGIT_H(0,0,0,1,1,0,0,0)},	// k
	{DIGIT_L(0,0,0,0,0,0,1,1), DIGIT_H(0,0,0,0,0,0,0,1)},	// l
	{DIGIT_L(1,1,1,0,1,0,1,0), DIGIT_H(0,0,0,0,0,0,0,0)},	// m
	{DIGIT_L(1,1,1,0,0,0,1,0), DIGIT_H(0,0,0,0,0,0,0,0)},	// n
	{DIGIT_L(1,1,1,0,0,0,1,1), DIGIT_H(0,0,0,0,0,0,0,0)},	// o
	{DIGIT_L(1,1,0,0,0,0,1,0), DIGIT_H(0,0,0,0,0,1,1,1)},	// p
	{DIGIT_L(1,1,1,0,0,0,0,0), DIGIT_H(0,0,0,0,0,1,1,1)},	// q
	{DIGIT_L(1,0,0,0,0,0,1,0), DIGIT_H(0,0,0,0,0,0,0,0)},	// r
	{DIGIT_L(1,1,1,0,0,0,0,1), DIGIT_H(0,0,0,0,0,1,0,1)},	// s
	{DIGIT_L(1,0,0,0,0,0,1,1), DIGIT_H(0,0,0,0,0,0,0,1)},	// t
	{DIGIT_L(0,0,1,0,0,0,1,1), DIGIT_H(0,0,0,0,0,0,0,0)},	// u
	{DIGIT_L(0,0,1,1,0,0,0,0), DIGIT_H(0,0,0,0,0,0,0,0)},	// v
	{DIGIT_L(0,0,1,1,0,1,1,0), DIGIT_H(0,0,0,0,0,0,0,0)},	// w
	{DIGIT_L(1,1,0,1,0,1,0,0), DIGIT_H(0,0,0,0,0,0,0,0)},	// x
	{DIGIT_L(0,0,0,0,0,1,0,0), DIGIT_H(0,0,0,1,1,0,0,0)},	// y
	{DIGIT_L(1,1,0,0,1,0,0,1), DIGIT_H(0,0,0,0,0,0,0,0)},	// z
                                                     
	{DIGIT_L(0,0,0,0,1,0,0,0), DIGIT_H(0,0,0,1,0,0,0,0)},	// |
	{DIGIT_L(0,0,0,0,0,0,0,1), DIGIT_H(0,0,0,0,0,0,0,0)}	// _
};

/* this array is used to display individual symbols
   the format is [byte position][bit to turn on] - both zero based */
static const char fipsymbols[NUM_SYMBOLS][2] = {
	{21,6}, 	/* AVC */
	{21,7}, 	/* NTSC*/
	{22,0}, 	/* WMT */
	{22,1}, 	/* PAL */
	{22,2}, 	/* MPEG4 */
	{22,3}, 	/* HD */
	{22,4},		/* DVD */
	{22,5}, 	/* DVI */
};

#endif
