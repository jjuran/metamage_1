/*
	Fonts.hh
	--------
*/

#ifndef FONTS_HH
#define FONTS_HH

typedef unsigned char Boolean;

struct FMInput;
struct FMOutput;

pascal void InitFonts_patch();

pascal void GetFNum_patch( const unsigned char* name, short* num );

pascal void SetFontLock_patch( Boolean locked );

pascal FMOutput* FMSwapFont_patch( const FMInput* input );

#endif
