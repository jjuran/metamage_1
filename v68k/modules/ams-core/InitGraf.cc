/*
	InitGraf.cc
	-----------
*/

#include "InitGraf.hh"

// Mac OS
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif


Ptr ScrnBase : 0x0824;


#define PACK16( _15, _14, _13,_12,  \
                _11, _10,  _9, _8,  \
                 _7,  _6,  _5, _4,  \
                 _3,  _2,  _1, _0 ) \
	(                            \
		+ (_15 << 15)  \
		| (_14 << 14)  \
		| (_13 << 13)  \
		| (_12 << 12)  \
		| (_11 << 11)  \
		| (_10 << 10)  \
		| ( _9 <<  9)  \
		| ( _8 <<  8)  \
		| ( _7 <<  7)  \
		| ( _6 <<  6)  \
		| ( _5 <<  5)  \
		| ( _4 <<  4)  \
		| ( _3 <<  3)  \
		| ( _2 <<  2)  \
		| ( _1 <<  1)  \
		| ( _0 <<  0)  \
	)

#define _ 0
#define X 1

#define ARROW_DATA  \
{  \
	PACK16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( _,X,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( _,X,X,_,_,_,_,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( _,X,X,X,_,_,_,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( _,X,X,X,X,_,_,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( _,X,X,X,X,X,_,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( _,X,X,X,X,X,X,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( _,X,X,X,X,X,X,X,_,_,_,_,_,_,_,_ ),  \
	PACK16( _,X,X,X,X,X,X,X,X,_,_,_,_,_,_,_ ),  \
	PACK16( _,X,X,X,X,X,_,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( _,X,X,_,X,X,_,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( _,X,_,_,_,X,X,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( _,_,_,_,_,X,X,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( _,_,_,_,_,_,X,X,_,_,_,_,_,_,_,_ ),  \
	PACK16( _,_,_,_,_,_,X,X,_,_,_,_,_,_,_,_ ),  \
	PACK16( _,_,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),  \
}

#define ARROW_MASK  \
{  \
	PACK16( X,X,_,_,_,_,_,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( X,X,X,_,_,_,_,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( X,X,X,X,_,_,_,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( X,X,X,X,X,_,_,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( X,X,X,X,X,X,_,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( X,X,X,X,X,X,X,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( X,X,X,X,X,X,X,X,_,_,_,_,_,_,_,_ ),  \
	PACK16( X,X,X,X,X,X,X,X,X,_,_,_,_,_,_,_ ),  \
	PACK16( X,X,X,X,X,X,X,X,X,X,_,_,_,_,_,_ ),  \
	PACK16( X,X,X,X,X,X,X,X,X,X,X,_,_,_,_,_ ),  \
	PACK16( X,X,X,X,X,X,X,_,_,_,_,_,_,_,_,_ ),  \
	PACK16( X,X,X,_,X,X,X,X,_,_,_,_,_,_,_,_ ),  \
	PACK16( X,X,_,_,X,X,X,X,_,_,_,_,_,_,_,_ ),  \
	PACK16( X,_,_,_,_,X,X,X,X,_,_,_,_,_,_,_ ),  \
	PACK16( _,_,_,_,_,X,X,X,X,_,_,_,_,_,_,_ ),  \
	PACK16( _,_,_,_,_,_,X,X,X,_,_,_,_,_,_,_ ),  \
}

static const Cursor arrow =
{
	ARROW_DATA,
	ARROW_MASK,
	{ 1, 1 }
};

static inline void set_Pattern( Pattern& pattern, uint32_t data )
{
	uint32_t* longword = (uint32_t*) &pattern;
	
	*longword++ = data;
	*longword   = data;
}

struct private_QDGlobals
{
	char privates[58];
	
	MacRegion  maxRegion;
	RgnPtr     maxRgnPtr;
	RgnHandle  maxRgnHandle;
	
	char publics[130];
};

pascal void InitGraf_patch( GrafPtr* thePort_ptr )
{
	asm
	{
		MOVE.L  thePort_ptr,(A5)
	}
	
	QDGlobals& qd = *(QDGlobals*) ((char*) thePort_ptr - offsetof(QDGlobals, thePort));
	
	private_QDGlobals& private_qd = (private_QDGlobals&) qd;
	
	private_qd.maxRegion.rgnSize = sizeof (MacRegion);
	private_qd.maxRegion.rgnBBox.top    = -32767;
	private_qd.maxRegion.rgnBBox.left   = -32767;
	private_qd.maxRegion.rgnBBox.bottom =  32767;
	private_qd.maxRegion.rgnBBox.right  =  32767;
	
	private_qd.maxRgnPtr    = &private_qd.maxRegion;
	private_qd.maxRgnHandle = &private_qd.maxRgnPtr;
	
	const unsigned width_px  = 512;
	const unsigned height_px = 342;
	
	const unsigned bit_depth = 1;  // bits per pixel
	
	const unsigned bits_per_byte = 8;
	
	qd.randSeed = 1;
	
	qd.screenBits.baseAddr = ScrnBase;
	qd.screenBits.rowBytes = width_px * bit_depth / bits_per_byte;
	qd.screenBits.bounds.top    = 0;
	qd.screenBits.bounds.left   = 0;
	qd.screenBits.bounds.bottom = height_px;
	qd.screenBits.bounds.right  = width_px;
	
	qd.arrow = arrow;
	
	set_Pattern( qd.dkGray, 0x77DD77DD );
	set_Pattern( qd.ltGray, 0x88228822 );
	set_Pattern( qd.gray,   0xAA55AA55 );
	set_Pattern( qd.black,  0xFFFFFFFF );
	set_Pattern( qd.white,  0x00000000 );
	
	qd.thePort = NULL;
}
