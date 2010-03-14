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
	
	struct GWorld_Context
	{
		GWorldPtr port;
		GDHandle gdh;
	};
	
	typedef GWorld_Context GetGWorld_Result;
	
}

namespace Nucleus
{
	
	template <>
	struct Maker< Nitrogen::GWorld_Context >
	{
		Nitrogen::GWorld_Context operator()( Nitrogen::GWorldPtr  port,
		                                     Nitrogen::GDHandle   gdh = NULL ) const
		{
			Nitrogen::GWorld_Context result;
			
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
	
	GWorld_Context GetGWorld();
	
	void SetGWorld( const GWorld_Context& state );
	void SetGWorld( GWorldPtr gWorld );
	
	GWorldFlags GetPixelsState( PixMapHandle pm );
	
	void SetPixelsState( PixMapHandle pm, GWorldFlags state );
	
	PixMapHandle GetGWorldPixMap( GWorldPtr offscreenGWorld );
	
	class GWorld
	{
		public:
			typedef GWorld_Context         value_type;
			typedef GWorld_Context const&  param_type;
			
			value_type get() const              { return GetGWorld(); }
			void set( param_type state ) const  { SetGWorld( state ); }
	};
	
	class Pixels_State
	{
		private:
			PixMapHandle pm;
		
		public:
			typedef GWorldFlags value_type;
			typedef GWorldFlags param_type;
			
			Pixels_State( PixMapHandle pm ) : pm( pm )  {}
			
			value_type get() const              { return Nitrogen::GetPixelsState( pm ); }
			void set( param_type state ) const  { SetPixelsState( pm, state );           }
	};
	
}

#endif

