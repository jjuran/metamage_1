// QDOffscreen.h

#ifndef NITROGEN_QDOFFSCREEN_H
#define NITROGEN_QDOFFSCREEN_H

#ifndef NITROGEN_FRAMEWORKHEADER_H
#include "Nitrogen/FrameworkHeader.h"
#endif
#ifndef __QDOFFSCREEN__
#include FRAMEWORK_HEADER(QD,QDOffscreen.h)
#endif
#ifndef NITROGEN_MACERRORS_H
#include "Nitrogen/MacErrors.h"
#endif
#ifndef NITROGEN_QUICKDRAW_H
#include "Nitrogen/QuickDraw.h"
#endif

namespace Nitrogen
{
	
	class GWorldFlags_Tag {};
	typedef FlagType< GWorldFlags_Tag, ::GWorldFlags > GWorldFlags;
	
	inline GWorldFlags NoNewDevice           ()  { return GWorldFlags::Make( noNewDevice             ); }
	inline GWorldFlags UseTempMem            ()  { return GWorldFlags::Make( useTempMem              ); }
	inline GWorldFlags KeepLocal             ()  { return GWorldFlags::Make( keepLocal               ); }
	inline GWorldFlags UseDistantHdwrMem     ()  { return GWorldFlags::Make( useDistantHdwrMem       ); }
	inline GWorldFlags UseLocalHdwrMem       ()  { return GWorldFlags::Make( useLocalHdwrMem         ); }
	inline GWorldFlags PixelsPurgeable       ()  { return GWorldFlags::Make( pixelsPurgeable         ); }
	inline GWorldFlags PixelsLocked          ()  { return GWorldFlags::Make( pixelsLocked            ); }
	inline GWorldFlags AllocDirectDrawSurface()  { return GWorldFlags::Make( kAllocDirectDrawSurface ); }
	// ...
	inline GWorldFlags GWFlagErr             ()  { return GWorldFlags::Make( gwFlagErr               ); }
	
	typedef CGrafPtr GWorldPtr;
	
	struct GWorld_State
	{
		GWorldPtr port;
		GDHandle gdh;
	};
	typedef GWorld_State GetGWorld_Result;
	
	struct GWorldDisposer : public std::unary_function< GWorldPtr, void >
	{
		void operator()( GWorldPtr gWorld ) const
		{
			::DisposeGWorld( gWorld );
		}
	};
	
	Owned< GWorldPtr, GWorldDisposer > NewGWorld(
		short pixelDepth, 
		const Rect& boundsRect, 
		CTabHandle cTable = NULL, 
		GDHandle aGDevice = NULL, 
		GWorldFlags flags = GWorldFlags()
	);
	
	Owned< GWorldPtr, GWorldDisposer > NewGWorld(
		short pixelDepth, 
		const Rect& boundsRect, 
		GWorldFlags flags
	);
	
	struct LockPixels_Failed {};
	
	void LockPixels(PixMapHandle pm);
	using ::UnlockPixels;
	
	GWorldFlags UpdateGWorld(
		Owned< GWorldPtr, GWorldDisposer >& offscreenGWorld, 
		short pixelDepth, 
		const Rect& boundsRect, 
		CTabHandle cTable = NULL, 
		GDHandle aGDevice = NULL, 
		GWorldFlags flags = GWorldFlags()
	);
	
	inline void DisposeGWorld( Owned< GWorldPtr, GWorldDisposer > )  {}
	
	GWorld_State GetGWorld();
	void SetGWorld( const GWorld_State& state );
	void SetGWorld( GWorldPtr gWorld );
	
	GWorldFlags GetPixelsState( PixMapHandle pm );
	void SetPixelsState( PixMapHandle pm, GWorldFlags state );
	
	PixMapHandle GetGWorldPixMap( GWorldPtr offscreenGWorld );
	
}

#endif
