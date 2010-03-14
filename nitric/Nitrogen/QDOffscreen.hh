// Nitrogen/QDOffscreen.hh
// -----------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2009 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NITROGEN_QDOFFSCREEN_HH
#define NITROGEN_QDOFFSCREEN_HH

#ifndef __QDOFFSCREEN__
#include <QDOffscreen.h>
#endif
#ifndef NITROGEN_QUICKDRAW_HH
#include "Nitrogen/Quickdraw.hh"
#endif

namespace Nitrogen
{
	
	enum GWorldFlags
	{
		kGWorldFlags_Max = nucleus::enumeration_traits< ::GWorldFlags >::max
	};
	
	NUCLEUS_DEFINE_FLAG_OPS( GWorldFlags )
	
	using ::GWorldPtr;
	
	struct GWorld_State
	{
		GWorldPtr port;
		GDHandle gdh;
	};
	
	typedef GWorld_State GetGWorld_Result;
	
}

namespace Nucleus
{
	
	template <>
	struct Maker< Nitrogen::GWorld_State >
	{
		Nitrogen::GWorld_State operator()( Nitrogen::GWorldPtr  port,
		                                   Nitrogen::GDHandle   gdh = NULL ) const
		{
			Nitrogen::GWorld_State result;
			
			result.port = port;
			result.gdh  = gdh;
			
			return result;
		}
	};
	
}

namespace Nitrogen
{
	
	nucleus::owned< GWorldPtr > NewGWorld( short        pixelDepth,
	                                       const Rect&  boundsRect,
	                                       CTabHandle   cTable   = NULL,
	                                       GDHandle     aGDevice = NULL,
	                                       GWorldFlags  flags    = GWorldFlags() );
	
	nucleus::owned< GWorldPtr > NewGWorld( short        pixelDepth,
	                                       const Rect&  boundsRect,
	                                       GWorldFlags  flags );
	
	struct LockPixels_Failed {};
	
	void LockPixels( PixMapHandle pm );
	
	using ::UnlockPixels;
	
	GWorldFlags UpdateGWorld( nucleus::owned< GWorldPtr >&  offscreenGWorld,
	                          short                         pixelDepth,
	                          const Rect&                   boundsRect,
	                          CTabHandle                    cTable   = NULL,
	                          GDHandle                      aGDevice = NULL,
	                          GWorldFlags                   flags    = GWorldFlags() );
	
	inline void DisposeGWorld( nucleus::owned< GWorldPtr > )  {}
	
	GWorld_State GetGWorld();
	
	void SetGWorld( const GWorld_State& state );
	void SetGWorld( GWorldPtr gWorld );
	
	GWorldFlags GetPixelsState( PixMapHandle pm );
	
	void SetPixelsState( PixMapHandle pm, GWorldFlags state );
	
	PixMapHandle GetGWorldPixMap( GWorldPtr offscreenGWorld );
	
	class GWorld_Value
	{
		public:
			typedef GWorld_State         Value;
			typedef GWorld_State         GetResult;
			typedef GWorld_State const&  SetParameter;
			
			static const bool hasSwap = false;
			
			GetResult Get() const                 { return GetGWorld(); }
			void Set( SetParameter state ) const  { SetGWorld( state ); }
	};
	
	class PixelsState_Value
	{
		private:
			PixMapHandle pm;
		
		public:
			typedef GWorldFlags Value;
			typedef GWorldFlags GetResult;
			typedef GWorldFlags SetParameter;
			
			static const bool hasSwap = false;
			
			PixelsState_Value( PixMapHandle pm ) : pm( pm )  {}
			
			GetResult Get() const                  { return Nitrogen::GetPixelsState( pm ); }
			void Set( SetParameter state ) const   { SetPixelsState( pm, state );           }
	};
	
}

#endif

