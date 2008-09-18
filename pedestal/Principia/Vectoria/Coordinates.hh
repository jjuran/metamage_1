/*	=======================
 *	Vectoria/Coordinates.hh
 *	=======================
 */

#ifndef VECTORIA_COORDINATES_HH
#define VECTORIA_COORDINATES_HH

#if PRAGMA_ONCE
#pragma once
#endif

#include "Vectoria/Matrix.hh"


namespace Vectoria
{
	
	struct XCoordinate
	{
		static const unsigned offset = 0;
		
		template < class Component >  Component const& operator()( Matrix< Component, 2, 1 > const& vector ) const  { return vector.data()[ offset ]; }
		template < class Component >  Component const& operator()( Matrix< Component, 3, 1 > const& vector ) const  { return vector.data()[ offset ]; }
		template < class Component >  Component const& operator()( Matrix< Component, 4, 1 > const& vector ) const  { return vector.data()[ offset ]; }
		
		template < class Component >  Component      & operator()( Matrix< Component, 2, 1 >      & vector )        { return vector.data()[ offset ]; }
		template < class Component >  Component      & operator()( Matrix< Component, 3, 1 >      & vector )        { return vector.data()[ offset ]; }
		template < class Component >  Component      & operator()( Matrix< Component, 4, 1 >      & vector )        { return vector.data()[ offset ]; }
	};
	
	struct YCoordinate
	{
		static const unsigned offset = 1;
		
		template < class Component >  Component const& operator()( Matrix< Component, 2, 1 > const& vector ) const  { return vector.data()[ offset ]; }
		template < class Component >  Component const& operator()( Matrix< Component, 3, 1 > const& vector ) const  { return vector.data()[ offset ]; }
		template < class Component >  Component const& operator()( Matrix< Component, 4, 1 > const& vector ) const  { return vector.data()[ offset ]; }
		
		template < class Component >  Component      & operator()( Matrix< Component, 2, 1 >      & vector )        { return vector.data()[ offset ]; }
		template < class Component >  Component      & operator()( Matrix< Component, 3, 1 >      & vector )        { return vector.data()[ offset ]; }
		template < class Component >  Component      & operator()( Matrix< Component, 4, 1 >      & vector )        { return vector.data()[ offset ]; }
	};
	
	struct ZCoordinate
	{
		static const unsigned offset = 2;
		
		template < class Component >  Component const& operator()( Matrix< Component, 3, 1 > const& vector ) const  { return vector.data()[ offset ]; }
		template < class Component >  Component const& operator()( Matrix< Component, 4, 1 > const& vector ) const  { return vector.data()[ offset ]; }
		
		template < class Component >  Component      & operator()( Matrix< Component, 3, 1 >      & vector )        { return vector.data()[ offset ]; }
		template < class Component >  Component      & operator()( Matrix< Component, 4, 1 >      & vector )        { return vector.data()[ offset ]; }
	};
	
	struct WCoordinate
	{
		template < class Component >  Component const& operator()( Matrix< Component, 3, 1 > const& vector ) const  { return vector.end()[ -1 ]; }
		template < class Component >  Component const& operator()( Matrix< Component, 4, 1 > const& vector ) const  { return vector.end()[ -1 ]; }
		
		template < class Component >  Component      & operator()( Matrix< Component, 3, 1 >      & vector )        { return vector.end()[ -1 ]; }
		template < class Component >  Component      & operator()( Matrix< Component, 4, 1 >      & vector )        { return vector.end()[ -1 ]; }
	};
	
	
	static const XCoordinate X = XCoordinate();
	static const YCoordinate Y = YCoordinate();
	static const ZCoordinate Z = ZCoordinate();
	static const WCoordinate W = WCoordinate();
	
}

#endif

