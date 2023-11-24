/*
	InitGraf.cc
	-----------
*/

#include "InitGraf.hh"

// Mac OS
#ifndef __FONTS__
#include <Fonts.h>
#endif
#ifndef __QUICKDRAW__
#include <Quickdraw.h>
#endif

// ams-common
#include "arrow.hh"
#include "callouts.hh"


short ScreenRow : 0x0106;
Ptr   ScrnBase  : 0x0824;
Rect  CrsrPin   : 0x0834;

Byte  QDExist   : 0x08F3;


static inline void set_Pattern( Pattern& pattern, uint32_t data )
{
	uint32_t* longword = (uint32_t*) &pattern;
	
	*longword++ = data;
	*longword   = data;
}

struct private_QDGlobals
{
	FMOutput   fontData;
	Ptr        fontPtr;
	Fixed      fontAdj;
	Point      patAlign;
	short      polyMax;
	Handle     thePoly;
	short      playIndex;
	long       playPic;
	short      rgnMax;
	short      rgnIndex;
	Handle     rgnBuf;
	MacRegion  wideData;
	RgnPtr     wideMaster;
	RgnHandle  wideOpen;
	
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
	
	private_qd.wideData.rgnSize = sizeof (MacRegion);
	private_qd.wideData.rgnBBox.top    = -32767;
	private_qd.wideData.rgnBBox.left   = -32767;
	private_qd.wideData.rgnBBox.bottom =  32767;
	private_qd.wideData.rgnBBox.right  =  32767;
	
	private_qd.wideMaster = &private_qd.wideData;
	private_qd.wideOpen   = &private_qd.wideMaster;
	
	qd.randSeed = 1;
	
	qd.screenBits.baseAddr = ScrnBase;
	qd.screenBits.rowBytes = ScreenRow;
	qd.screenBits.bounds   = CrsrPin;
	
	fast_memcpy( &qd.arrow, &arrow, sizeof arrow );
	
	set_Pattern( qd.dkGray, 0x77DD77DD );
	set_Pattern( qd.ltGray, 0x88228822 );
	set_Pattern( qd.gray,   0xAA55AA55 );
	set_Pattern( qd.black,  0xFFFFFFFF );
	set_Pattern( qd.white,  0x00000000 );
	
	qd.thePort = NULL;
	
	QDExist = 0;
}
