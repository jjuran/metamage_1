/*	=======================
 *	Vectoria/PlaneVector.hh
 *	=======================
 */

#ifndef VECTORIA_PLANEVECTOR_HH
#define VECTORIA_PLANEVECTOR_HH

// Vertice
#include "Vectoria/CrossProduct3D.hh"
#include "Vectoria/Plane3D.hh"
#include "Vectoria/Point3D.hh"
#include "Vectoria/Vector.hh"


namespace Vectoria
{
	
	template < class Points >
	inline Vector3D::Type FaceNormal( const Points& points )
	{
		return CrossProduct( points[ 1 ] - points[ 0 ],
		                     points[ 2 ] - points[ 0 ] );
	}
	
	inline double ProportionalDistanceFromOrigin( const Vector3D::Type&  normal,
	                                              const Point3D::Type&   pt )
	{
		return DotProduct( -normal, pt );
	}
	
	inline Plane3D::Type PlaneVector( const Vector3D::Type&  normal,
	                                  const Point3D::Type&   pointInPlane )
	{
		double D = ProportionalDistanceFromOrigin( normal, pointInPlane );
		
		return Plane3D::Make( normal, D );
	}
	
	template < class Points >
	inline Plane3D::Type PlaneVector( const Points& points )
	{
		Vector3D::Type normal = FaceNormal( points );
		
		double D = ProportionalDistanceFromOrigin( normal, points[ 0 ] );
		
		return Plane3D::Make( normal, D );
	}
	
}

#endif
