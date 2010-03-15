/*	============================
 *	Pedestal/GeneratedGraphic.hh
 *	============================
 */

#ifndef PEDESTAL_GENERATEDGRAPHIC_HH
#define PEDESTAL_GENERATEDGRAPHIC_HH

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// Pedestal
#include "Pedestal/Graphic.hh"


namespace Pedestal
{
	
	inline UInt16 DenormalizeUInt16( double x )  { return UInt16( 65535.0 * x ); }
	
	inline RGBColor DenormalizeRGBColor( double r, double g, double b )
	{
		return nucleus::make< RGBColor >( DenormalizeUInt16( r ),
		                                  DenormalizeUInt16( g ),
		                                  DenormalizeUInt16( b ) );
	}
	
	inline RGBColor DenormalizeRGBColor( double grey )
	{
		return DenormalizeRGBColor( grey, grey, grey );
	}
	
	class RGBConstant
	{
		private:
			RGBColor fColor;
		
		public:
			RGBConstant( const RGBColor& color ) : fColor( color )  {}
			
			const RGBColor& operator()( double /*x*/, double /*y*/ ) const
			{
				return fColor;
			}
	};
	
	class XYAverage
	{
		public:
			RGBColor operator()( double x, double y ) const
			{
				return DenormalizeRGBColor( (x + y) / 2 );
			}
	};
	
	class XTimesY
	{
		public:
			RGBColor operator()( double x, double y ) const
			{
				return DenormalizeRGBColor( x * y );
			}
	};
	
	template < class Function >
	class GeneratedGraphic : public Graphic
	{
		private:
			Function fFunction;
		
		public:
			typedef Function Initializer;
			
			GeneratedGraphic( const Function& f = Function() ) : fFunction( f )  {}
			
			void Plot( const Rect& area ) const;
	};
	
	template < class Function >
	void GeneratedGraphic< Function >::Plot( const Rect& area ) const
	{
		double width  = area.right  - area.left - 1;
		double height = area.bottom - area.top - 1;
		
		for ( short v = area.top;  v < area.bottom;  ++v )
		{
			double y = 1.0 - (v - area.top) / height;
			
			for ( short h = area.left;  h < area.right;  ++ h )
			{
				double x = (h - area.left) / width;
				
				Nitrogen::SetCPixel( h,
				                     v,
				                     fFunction( x,
				                                y,
				                                0 ) );
			}
		}
	}
	
}

#endif

