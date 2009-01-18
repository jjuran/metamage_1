// Nitrogen/QDOffscreen.cp
// -----------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2004-2007 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#include "Nitrogen/QDOffscreen.h"


namespace Nitrogen
{
	
	namespace Function
	{
	#if TARGET_CPU_68K && !TARGET_RT_MAC_CFM
		
		static pascal void DisposeGWorld( GWorldPtr world )  { ::DisposeGWorld( world ); }
		
	#else
		
		using ::DisposeGWorld;
		
	#endif
	}
	
	Nucleus::Owned< GWorldPtr > NewGWorld( short        pixelDepth,
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
		
		return Nucleus::Owned< GWorldPtr >::Seize( newWorld, &Function::DisposeGWorld );
	}
	
	Nucleus::Owned< GWorldPtr > NewGWorld( short        pixelDepth,
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
	
	GWorldFlags UpdateGWorld( Nucleus::Owned< GWorldPtr >&  offscreenGWorld,
	                          short                         pixelDepth,
	                          const Rect&                   boundsRect,
	                          CTabHandle                    cTable,
	                          GDHandle                      aGDevice,
	                          GWorldFlags                   flags )
	{
		::GWorldPtr gWorldPtr = offscreenGWorld.Get();
		
		GWorldFlags result = GWorldFlags( ::UpdateGWorld( &gWorldPtr,
		                                                  pixelDepth,
		                                                  &boundsRect,
		                                                  cTable,
		                                                  aGDevice,
		                                                  flags ) );
		
		if ( result & gwFlagErr )
		{
			ThrowOSStatus( ::QDError() );
		}
		
		if ( gWorldPtr != offscreenGWorld.Get() )
		{
			offscreenGWorld.Release();
			
			offscreenGWorld = Nucleus::Owned< GWorldPtr >::Seize( gWorldPtr, &Function::DisposeGWorld );
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

