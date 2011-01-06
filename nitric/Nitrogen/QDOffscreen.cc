// Nitrogen/QDOffscreen.cc
// -----------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/QDOffscreen.hh"

// MacGlue
#include "MacGlue/MacGlue.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"


namespace MacGlue
{
	
	DECLARE_MAC_GLUE( DisposeGWorld );
	
}

namespace Nitrogen
{
	
	using Mac::ThrowOSStatus;
	
	
	nucleus::owned< GWorldPtr > NewGWorld( short        pixelDepth,
	                                       const Rect&  boundsRect,
	                                       CTabHandle   cTable,
	                                       GDHandle     aGDevice,
	                                       GWorldFlags  flags )
	{
		::GWorldPtr newWorld = NULL;
		
		ThrowOSStatus( ::NewGWorld( &newWorld,
		                            pixelDepth,
		                            &boundsRect,
		                            cTable,
		                            aGDevice,
		                            flags ) );
		
		return nucleus::owned< GWorldPtr >::seize( newWorld, &MacGlue::DisposeGWorld );
	}
	
	nucleus::owned< GWorldPtr > NewGWorld( short        pixelDepth,
	                                       const Rect&  boundsRect,
	                                       GWorldFlags  flags )
	{
		return NewGWorld( pixelDepth, boundsRect, NULL, NULL, flags );
	}
	
	void LockPixels( PixMapHandle pm )
	{
		bool locked = ::LockPixels( pm );
		
		if ( !locked )
		{
			throw LockPixels_Failed();
		}
	}
	
	GWorldFlags UpdateGWorld( nucleus::owned< GWorldPtr >&  offscreenGWorld,
	                          short                         pixelDepth,
	                          const Rect&                   boundsRect,
	                          CTabHandle                    cTable,
	                          GDHandle                      aGDevice,
	                          GWorldFlags                   flags )
	{
		::GWorldPtr gWorldPtr = offscreenGWorld.get();
		
		GWorldFlags result = GWorldFlags( ::UpdateGWorld( &gWorldPtr,
		                                                  pixelDepth,
		                                                  &boundsRect,
		                                                  cTable,
		                                                  aGDevice,
		                                                  flags ) );
		
		if ( result & gwFlagErr )
		{
			ThrowOSStatus( ::OSStatus( result ) );
		}
		
		if ( gWorldPtr != offscreenGWorld.get() )
		{
			offscreenGWorld.release();
			
			offscreenGWorld = nucleus::owned< GWorldPtr >::seize( gWorldPtr, &MacGlue::DisposeGWorld );
		}
		
		return result;
	}
	
	GWorld_Context GetGWorld()
	{
		::GWorldPtr port;
		::GDHandle gdh;
		
		::GetGWorld( &port, &gdh );
		
		GWorld_Context gworld;
		
		gworld.port = port;
		gworld.gdh  = gdh;
		
		return gworld;
	}
	
	void SetGWorld( const GWorld_Context& state )
	{
		::SetGWorld( state.port, state.gdh );
	}
	
	void SetGWorld( GWorldPtr gWorld )
	{
		::SetGWorld( gWorld, NULL );
	}
	
	GWorldFlags GetPixelsState( PixMapHandle pm )
	{
		return GWorldFlags( ::GetPixelsState( pm ) );
	}
	
	void SetPixelsState( PixMapHandle pm, GWorldFlags state )
	{
		::SetPixelsState( pm, state );
	}
	
	PixMapHandle GetGWorldPixMap( GWorldPtr offscreenGWorld )
	{
		return ::GetGWorldPixMap( offscreenGWorld );
	}
	
}

