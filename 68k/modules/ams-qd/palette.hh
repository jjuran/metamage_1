/*
	palette.hh
	----------
*/

#ifndef PALETTE_HH
#define PALETTE_HH

struct BitMap;
struct ColorSpec;

typedef unsigned char Byte;

void polarize_palette( const ColorSpec* palette, short size, Byte* polarized );

void apply_palette( const BitMap& bits, const Byte* polarized_palette );

#endif
