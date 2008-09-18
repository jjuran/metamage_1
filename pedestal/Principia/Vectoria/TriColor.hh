/*	====================
 *	Vectoria/TriColor.hh
 *	====================
 */

#ifndef VECTORIA_TRICOLOR_HH
#define VECTORIA_TRICOLOR_HH

#if PRAGMA_ONCE
#pragma once
#endif

// Vectoria
#include "Vectoria/Matrix.hh"


namespace Vectoria
{
	
	template < unsigned offset >
	struct ColorChannel
	{
		//template < class Component >  Component const& operator()( Matrix< Component, 2, 1 > const& vector ) const  { return vector.data()[ offset ]; }
		template < class Component >  Component const& operator()( Matrix< Component, 3, 1 > const& vector ) const  { return vector.data()[ offset ]; }
		//template < class Component >  Component const& operator()( Matrix< Component, 4, 1 > const& vector ) const  { return vector.data()[ offset ]; }
		
		//template < class Component >  Component      & operator()( Matrix< Component, 2, 1 >      & vector )        { return vector.data()[ offset ]; }
		template < class Component >  Component      & operator()( Matrix< Component, 3, 1 >      & vector )        { return vector.data()[ offset ]; }
		//template < class Component >  Component      & operator()( Matrix< Component, 4, 1 >      & vector )        { return vector.data()[ offset ]; }
	};
	
	typedef ColorChannel< 0 > RedChannel;
	typedef ColorChannel< 1 > GreenChannel;
	typedef ColorChannel< 2 > BlueChannel;
	
	static const RedChannel   Red   = RedChannel();
	static const GreenChannel Green = GreenChannel();
	static const BlueChannel  Blue  = BlueChannel();
	
	
	template < class Component >
	inline Matrix< Component, 3, 1 > MakeRGB( Component red, Component green, Component blue )
	{
		Matrix< Component, 3, 1 > color;
		
		color[ Red   ] = red;
		color[ Green ] = green;
		color[ Blue  ] = blue;
		
		return color;
	}
	
	template < class Component >
	inline Matrix< Component, 3, 1 > MakeGray( Component gray )
	{
		return MakeRGB( gray, gray, gray );
	}
	
	enum IntensityPole
	{
		kBlackPole = 0,
		kWhitePole = 1
	};
	
	template < IntensityPole intensity >
	struct ColorExtremum
	{
		template < class Component >
		Matrix< Component, 3, 1 >& Initialize( Matrix< Component, 3, 1 >& matrix ) const
		{
			return matrix = MakeGray< Component >( intensity );
		}
	};
	
	typedef ColorExtremum< kBlackPole > Black;
	typedef ColorExtremum< kWhitePole > White;
	
}

#endif

