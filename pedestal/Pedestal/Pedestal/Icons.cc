/*	========
 *	Icons.cc
 *	========
 */

#include "Pedestal/Icons.hh"

// Nitrogen
#include "Nitrogen/Icons.h"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	template < class Icon >
	struct PlotIcon_Traits;
	
	template <>
	struct PlotIcon_Traits< N::PlainIconHandle >
	{
		static void PlotIcon( const Rect&           area,
	                          N::IconAlignmentType  align,
	                          N::IconTransformType  transform,
	                          N::PlainIconHandle    icon )
		{
			N::PlotIconHandle( area, align, transform, icon );
		}
	};
	
	template <>
	struct PlotIcon_Traits< N::MaskedIconHandle >
	{
		static void PlotIcon( const Rect&           area,
	                          N::IconAlignmentType  align,
	                          N::IconTransformType  transform,
	                          N::MaskedIconHandle   icon )
		{
			N::PlotIconHandle( area, align, transform, icon );
		}
	};
	
	template <>
	struct PlotIcon_Traits< N::ResID >
	{
		static void PlotIcon( const Rect&           area,
	                          N::IconAlignmentType  align,
	                          N::IconTransformType  transform,
	                          N::ResID              resID )
		{
			N::PlotIconID( area, align, transform, resID );
		}
	};
	
	static NN::Owned< N::PlainIconHandle > GetIconMask( N::MaskedIconHandle maskedIcon )
	{
		NN::Owned< N::PlainIconHandle > result = N::NewHandleArray< N::PlainIcon >();
		
		**result.Get() = (**maskedIcon).mask;
		
		return result;
	}
	
	static Rect LargeIconRect()
	{
		return N::SetRect( 0, 0, 32, 32 );
	}
	
	template < class Icon >
	static void CopyIconToLockedGWorld( Icon icon, N::GWorldPtr gWorld )
	{
		N::GWorld_State savedGWorld = N::GetGWorld();
		
		N::SetGWorld( gWorld );
		
		Rect bounds = N::GetPortBounds( gWorld );
		
		N::EraseRect( bounds );
		
		PlotIcon_Traits< Icon >::PlotIcon( bounds, icon );
		
		N::SetGWorld( savedGWorld );
	}
	
	template < class Icon >
	static void CopyIconToGWorld( Icon icon, N::GWorldPtr gWorld )
	{
		// Lock pixels
		PixMapHandle pix = N::GetGWorldPixMap( gWorld );
		N::Saved< N::PixelsState_Value > savedPixelsState( pix );
		
		N::LockPixels( pix );
		
		CopyIconToLockedGWorld( icon, gWorld );
	}
	
	template < class Icon >
	static NN::Owned< N::GWorldPtr, N::GWorldDisposer > MakeGWorldFromIcon( Icon icon )
	{
		NN::Owned< N::GWorldPtr, N::GWorldDisposer > gWorld = N::NewGWorld( 1, LargeIconRect() );
		
		CopyPlainIconToGWorld( icon, gWorld );
		
		return gWorld;
	}
	
	void PlainIcon::Plot( const Rect& area ) const
	{
		N::PlotIconHandle( area,
		                   kAlignNone,
		                   kTransformNone,
		                   value );
	}
	
	void MaskedIcon::Plot( const Rect& area ) const
	{
		N::PlotIconHandle( area,
		                   kAlignNone,
		                   kTransformNone,
		                   value );
	}
	
	#if 0
	void MaskedIcon::Plot( const Rect& area ) const
	{
		// Make a new icon handle for the mask
		NN::Owned< N::PlainIconHandle > mask = GetIconMask( fIcon );
		
		NN::Owned< N::GWorldPtr, N::GWorldDisposer > gWorld = N::NewGWorld( 1, LargeIconRect() );
		
		// Lock pixels
		N::PixelsState_Details details( N::GetGWorldPixMap( gWorld ) );
		NN::Scoped< N::PixelsState > savedPixelsState( N::PixelsState( details ) );
		
		N::LockPixels( N::GetGWorldPixMap( gWorld ) );
		
		CopyIconToLockedGWorld( mask.Get(), gWorld );
		
		N::CopyBits( N::GetPortBitMapForCopyBits( gWorld                   ),
		             N::GetPortBitMapForCopyBits( N::GetQDGlobalsThePort() ),
		             N::GetPortBounds( gWorld ),
		             area,
		             srcBic );
		
		CopyIconToLockedGWorld( N::Handle_Cast< N::PlainIcon >( N::Handle( Get() ) ),
		                             gWorld );
		
		N::CopyBits( N::GetPortBitMapForCopyBits( gWorld                   ),
		             N::GetPortBitMapForCopyBits( N::GetQDGlobalsThePort() ),
		             N::GetPortBounds( gWorld ),
		             area,
		             srcOr );
		
	}
	#endif
	
	void IconID::Plot( const Rect& area ) const
	{
		N::PlotIconID( area,
		               kAlignNone,
		               kTransformNone,
		               value );
	}
	
}

