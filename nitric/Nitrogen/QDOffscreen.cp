// QDOffscreen.cp

#ifndef NITROGEN_QDOFFSCREEN_H
#include "Nitrogen/QDOffscreen.h"
#endif

namespace Nitrogen
{
	
	Nucleus::Owned< GWorldPtr, GWorldDisposer > NewGWorld( short pixelDepth,
	                                              const Rect& boundsRect,
	                                              CTabHandle cTable,
	                                              GDHandle aGDevice,
	                                              GWorldFlags flags )
	{
		::GWorldPtr newWorld = NULL;
		
		ThrowOSStatus( ::NewGWorld( &newWorld,
		                            pixelDepth,
		                            &boundsRect,
		                            cTable,
		                            aGDevice,
		                            flags ) );
		
		return Nucleus::Owned< GWorldPtr, GWorldDisposer >::Seize( newWorld );
	}
	
	Nucleus::Owned< GWorldPtr, GWorldDisposer > NewGWorld( short pixelDepth,
	                                              const Rect& boundsRect,
	                                              GWorldFlags flags )
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
	
	GWorldFlags UpdateGWorld( Nucleus::Owned< GWorldPtr, GWorldDisposer >&  offscreenGWorld,
	                          short                                pixelDepth,
	                          const Rect&                          boundsRect,
	                          CTabHandle                           cTable,
	                          GDHandle                             aGDevice,
	                          GWorldFlags                          flags )
	{
		::GWorldPtr gWorldPtr = offscreenGWorld.Get();
		
		GWorldFlags result( ::UpdateGWorld( &gWorldPtr,
		                                    pixelDepth,
		                                    &boundsRect,
		                                    cTable,
		                                    aGDevice,
		                                    flags ) );
		
		if ( result.Get() & gwFlagErr )
		{
			ThrowOSStatus( ::QDError() );
		}
		
		if ( gWorldPtr != offscreenGWorld.Get() )
		{
			offscreenGWorld.Release();
			offscreenGWorld = Nucleus::Owned< GWorldPtr, GWorldDisposer >::Seize( gWorldPtr );
		}
		
		return result;
	}
	
	GWorld_State GetGWorld()
	{
		::GWorldPtr port;
		::GDHandle gdh;
		
		::GetGWorld( &port, &gdh );
		
		GWorld_State state;
		
		state.port = GWorldPtr( port );
		state.gdh  = GDHandle( gdh );
		
		return state;
	}
	
	void SetGWorld( const GWorld_State& state )
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

