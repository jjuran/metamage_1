/*	==================
 *	Vectoria/Vector.hh
 *	==================
 */

#ifndef VECTORIA_VECTOR_HH
#define VECTORIA_VECTOR_HH

// Standard C++
#include <numeric>

// Standard C/C++
#include <cmath>

// Vectoria
#include "Vectoria/Matrix.hh"


namespace Vectoria
{
	
	template < class Component, unsigned Length >
	struct Vector
	{
		typedef Matrix< Component, Length, 1 > Type;
	};
	
	template < class Component, unsigned Length >
	unsigned Dimensionality( const Matrix< Component, Length, 1 >& )
	{
		return Length;
	}
	
	template < class Component, unsigned Length >
	Component DotProduct( const Matrix< Component, Length, 1 >& a,
	                      const Matrix< Component, Length, 1 >& b )
	{
		return std::inner_product( a.begin(),
		                           a.end(),
		                           b.begin(),
		                           Component( 0 ) );
	}
	
	template < class Component, unsigned Length >
	Component MagnitudeSquared( const Matrix< Component, Length, 1 >& v )
	{
		return DotProduct( v, v );
	}
	
	template < class Component, unsigned Length >
	Component Magnitude( const Matrix< Component, Length, 1 >& v )
	{
		return std::sqrt( MagnitudeSquared( v ) );
	}
	
	template < class Component, unsigned Length >
	Matrix< Component, Length, 1 > UnitLength( const Matrix< Component, Length, 1 >& v )
	{
		return v / Magnitude( v );
	}
	
	template < class Component, unsigned Length >
	Matrix< Component, Length, 1 > Projection( const Matrix< Component, Length, 1 >&  P,
	                                           const Matrix< Component, Length, 1 >&  Q )
	{
		return DotProduct( P, Q ) / MagnitudeSquared( Q ) * Q;
	}
	
	template < class Component, unsigned Length >
	Matrix< Component, Length, 1 > Perpendicular( const Matrix< Component, Length, 1 >&  P,
	                                              const Matrix< Component, Length, 1 >&  Q )
	{
		return P - Projection( P, Q );
	}
	
}

#endif

