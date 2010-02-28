/*	=================
 *	GrafX/RGBColor.hh
 *	=================
 */

#ifndef GRAFX_RGBCOLOR_HH
#define GRAFX_RGBCOLOR_HH

// Nucleus
#include "Nucleus/Convert.h"
#include "Nucleus/Make.h"

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

namespace Nucleus
{
	
	template <> struct Converter< GrafX::Color, ::RGBColor >: public std::unary_function< ::RGBColor, GrafX::Color >
	{
		GrafX::Color operator()( const ::RGBColor& color ) const
		{
			using GrafX::gMaxIntensity;
			
			return Vectoria::MakeRGB( GrafX::Normalize< gMaxIntensity >( color.red   ),
			                          GrafX::Normalize< gMaxIntensity >( color.green ),
			                          GrafX::Normalize< gMaxIntensity >( color.blue  ) );
		}
	};
	
	template <> struct Converter< ::RGBColor, GrafX::Color >: public std::unary_function< GrafX::Color, ::RGBColor >
	{
		::RGBColor operator()( const GrafX::Color& color ) const
		{
			using GrafX::gMaxIntensity;
			
			return Make< ::RGBColor >( GrafX::Denormalize< gMaxIntensity >( color[ Vectoria::Red   ] ),
			                           GrafX::Denormalize< gMaxIntensity >( color[ Vectoria::Green ] ),
			                           GrafX::Denormalize< gMaxIntensity >( color[ Vectoria::Blue  ] ) );
		}
	};
	
}

#endif

