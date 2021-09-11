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

pascal void GetFName_patch( short num, unsigned char* name );

pascal void GetFNum_patch( const unsigned char* name, short* num );

pascal unsigned char RealFont_patch( short num, short size );

pascal void SetFontLock_patch( Boolean locked );

pascal FMOutput* FMSwapFont_patch( const FMInput* input );

pascal void SetFScaleDisable_patch( Boolean disable );

#endif
