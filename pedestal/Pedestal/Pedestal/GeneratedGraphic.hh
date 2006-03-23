/*	============================
 *	Pedestal/GeneratedGraphic.hh
 *	============================
 */

#pragma once

// Pedestal
#include "Pedestal/Graphic.hh"


namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	class RGBConstant
	{
		private:
			RGBColor fColor;
		
		public:
			RGBConstant( const RGBColor& color ) : fColor( color )  {}
			
			const RGBColor& operator()( double x, double y ) const
			{
				return fColor;
			}
	};
	
	class XYAverage
	{
		public:
			RGBColor operator()( double x, double y ) const
			{
				return NN::Make< RGBColor >( (x + y) / 2 * 65535.0 );
			}
	};
	
	class XTimesY
	{
		public:
			RGBColor operator()( double x, double y ) const
			{
				return NN::Make< RGBColor >( (x * y) * 65535.0 );
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
				
				N::SetCPixel( h,
				              v,
				              fFunction( x,
				                         y,
				                         0 ) );
			}
		}
	}
	
}

