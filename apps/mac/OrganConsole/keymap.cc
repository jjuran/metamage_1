/*
	keymap.cc
	---------
*/

#include "keymap.hh"


#define MIDI(note)  (256 + (note))

#define C  MIDI(60)
#define Db (C + 1)
#define D  (C + 2)
#define Eb (C + 3)
#define E  (C + 4)
#define F  (C + 5)
#define Gb (C + 6)
#define G  (C + 7)
#define Ab (C + 8)
#define A  (C + 9)
#define Bb (C + 10)
#define B  (C + 11)

#define BASE  0

#define SUPRA  BASE + 12 +
#define UPPER  BASE      +
#define LOWER  BASE - 12 +
#define BASSO  BASE - 24 +

/*
	Upper manual:
	
	      Db  Eb      Gb  Ab  Bb      Db  Eb
	      4   5       7   8   9       -   =
	      21  23      26  28  25      27  24
	B   C   D   E   F   G   A   B   C   D   E   F
	w   e   r   t   y   u   i   o   p   [   ]   \
	13  14  15  17  16  32  34  31  35  33  30  42
	
	Lower manual:
	
	      Db  Eb      Gb  Ab  Bb      Db
	      d   f       h   j   k       ;
	      2   3       4   38  40      41
	B   C   D   E   F   G   A   B   C   D
	z   x   c   v   b   n   m   ,   .   /
	6   7   8   9   11  45  46  43  47  44
	
*/

// Map virtual key codes to MIDI notes (or other functions):

short keymap[ 64 ] =
{
	// 0 - 15
	
	0,
	0,
	LOWER Db,  // D
	LOWER Eb,  // F
	LOWER Gb,  // H
	0,
	BASSO B,   // Z
	LOWER C,   // X
	LOWER D,   // C
	LOWER E,   // V
	0,
	LOWER F,   // B
	0,
	LOWER B,   // W
	UPPER C,   // E
	UPPER D,   // R
	
	// 16 - 31
	
	UPPER F,   // Y
	UPPER E,   // T
	0,
	0,
	0,
	UPPER Db,  // 4
	0,
	UPPER Eb,  // 5
	SUPRA Eb,  // =
	UPPER Bb,  // 9
	UPPER Gb,  // 7
	SUPRA Db,  // -
	UPPER Ab,  // 8
	0,
	SUPRA E,   // ]
	UPPER B,   // O
	
	// 32 - 47
	
	UPPER G,   // U
	SUPRA D,   // [
	UPPER A,   // I
	SUPRA C,   // P
	0,
	0,
	LOWER Ab,  // J
	0,
	LOWER Bb,  // K
	UPPER Db,  // ;
	SUPRA F,   /* \ */
	LOWER B,   // ,
	UPPER D,   // /
	LOWER G,   // N
	LOWER A,   // M
	UPPER C,   // .
};
