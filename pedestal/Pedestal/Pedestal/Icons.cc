/*	========
 *	Icons.cc
 *	========
 */

#include "Pedestal/Icons.hh"


namespace Nitrogen
{
	
	static void PlotIconHandle( const Rect&        area,
	                            IconAlignmentType  align,
	                            IconTransformType  transform,
	                            Handle             icon )
	{
		ThrowOSStatus( ::PlotIconHandle( &area, align, transform, icon ) );
	}
	
}

namespace Pedestal
{
	
	namespace N = Nitrogen;
	
	
	void PlainIcon::Draw( const Rect& bounds, bool erasing )
	{
		if ( N::Handle data = Data() )
		{
			if ( erasing )
			{
				N::EraseRect( bounds );
			}
			
			N::PlotIconHandle( bounds, Alignment(), Transform(), data );
		}
	}
	
	
	void IconID::Draw( const Rect& bounds, bool erasing )
	{
		if ( erasing )
		{
			N::EraseRect( bounds );
		}
		
		try
		{
			N::PlotIconID( bounds, Alignment(), Transform(), ID() );
		}
		catch ( const Undefined& )
		{
		}
	}
	
	
	void IconSuite::Draw( const Rect& bounds, bool erasing )
	{
		if ( N::IconSuiteRef ref = Ref() )
		{
			if ( erasing )
			{
				N::EraseRect( bounds );
			}
			
			N::PlotIconSuite( bounds, Alignment(), Transform(), ref );
		}
	}
	
}

