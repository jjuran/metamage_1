/*	=================
 *	GrafX/RGBColor.hh
 *	=================
 */

#ifndef GRAFX_RGBCOLOR_HH
#define GRAFX_RGBCOLOR_HH

// nucleus
#ifndef NUCLEUS_CONVERT_HH
#include "nucleus/convert.hh"
#endif
#ifndef NUCLEUS_MAKE_HH
#include "nucleus/make.hh"
#endif

// Nitrogen
#include "Nitrogen/Quickdraw.hh"

// Vectoria
#include "Vectoria/TriColor.hh"


namespace GrafX
{
	
	typedef Vectoria::Matrix< double, 3, 1 > Color;
	
	const UInt16 gMaxIntensity = 65535;
	
	template < UInt16 Max >
	inline double Normalize( double intensity )
	{
		return intensity / Max;
	}
	
	template < UInt16 Max >
	inline UInt16 Denormalize( double intensity )
	{
		return (intensity < 1.0) ? UInt16( intensity * Max ) : Max;
	}
	
}

namespace nucleus
{
	
	template <> struct converter< GrafX::Color, ::RGBColor >
	{
		typedef ::RGBColor    argument_type;
		typedef GrafX::Color  result_type;
		
		GrafX::Color operator()( const ::RGBColor& color ) const
		{
			using GrafX::gMaxIntensity;
			
			return Vectoria::MakeRGB( GrafX::Normalize< gMaxIntensity >( color.red   ),
			                          GrafX::Normalize< gMaxIntensity >( color.green ),
			                          GrafX::Normalize< gMaxIntensity >( color.blue  ) );
		}
	};
	
	template <> struct converter< ::RGBColor, GrafX::Color >
	{
		typedef GrafX::Color  argument_type;
		typedef ::RGBColor    result_type;
		
		::RGBColor operator()( const GrafX::Color& color ) const
		{
			using GrafX::gMaxIntensity;
			
			return make< ::RGBColor >( GrafX::Denormalize< gMaxIntensity >( color[ Vectoria::Red   ] ),
			                           GrafX::Denormalize< gMaxIntensity >( color[ Vectoria::Green ] ),
			                           GrafX::Denormalize< gMaxIntensity >( color[ Vectoria::Blue  ] ) );
		}
	};
	
}

#endif

