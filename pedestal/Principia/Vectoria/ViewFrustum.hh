/*	=======================
 *	Vectoria/ViewFrustum.hh
 *	=======================
 */

#ifndef VECTORIA_VIEWFRUSTUM_HH
#define VECTORIA_VIEWFRUSTUM_HH

// Standard C/C++
#include <cmath>

// Vertice
#include "Vectoria/Plane3D.hh"


namespace Vectoria
{
	
	// Legend:
	// n - unsigned near plane distance
	// f - unsigned far plane distance
	// e - focal length
	// a - aspect ratio
	
	inline Plane3D::Type NearPlane( double n )
	{
		return Plane3D::Make( 0, 0, -1, -n );
	}
	
	inline Plane3D::Type FarPlane( double f )
	{
		return Plane3D::Make( 0, 0, 1, f );
	}
	
	inline Plane3D::Type LeftPlane( double e )
	{
		double denom = std::sqrt( e*e + 1 );
		
		return Plane3D::Make( e / denom,
		                      0,
		                      -1 / denom,
		                      0 );
	}
	
	inline Plane3D::Type RightPlane( double e )
	{
		double denom = std::sqrt( e*e + 1 );
		
		return Plane3D::Make( -e / denom,
		                      0,
		                      -1 / denom,
		                      0 );
	}
	
	inline Plane3D::Type BottomPlane( double e, double a )
	{
		double denom = std::sqrt( e*e + a*a );
		
		return Plane3D::Make( 0,
		                      e / denom,
		                      -a / denom,
		                      0 );
	}
	
	inline Plane3D::Type TopPlane( double e, double a )
	{
		double denom = std::sqrt( e*e + a*a );
		
		return Plane3D::Make( 0,
		                      -e / denom,
		                      -a / denom,
		                      0 );
	}
	
}

#endif

