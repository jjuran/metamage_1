/*	===========================
 *	Vectoria/LinearAlgebra3D.hh
 *	===========================
 */

#ifndef VECTORIA_LINEARALGEBRA3D_HH
#define VECTORIA_LINEARALGEBRA3D_HH

#include "Vectoria/Transform.hh"
#include "Vectoria/Point2D.hh"
#include "Vectoria/Point3D.hh"
#include "Vectoria/Plane3D.hh"


namespace Vectoria
{
	
	template < class Vector, class Plane >
	bool LineIntersectsPlane( Vector& V, const Plane& L )
	{
		return DotProduct( L, V ) != 0;
	}
	
	template < class Vector, class Point, class Plane >
	Point LinePlaneIntersection( const Vector& V, const Point& Q, const Plane& L )
	{
		// It is assumed that the line intersects the plane.
		// If it doesn't, there will be a division by zero.
		
		double t =   - DotProduct( L, Q )
		           /   DotProduct( L, V );
		
		Point Pt = Q + t * V;
		
		return Pt;
	}
	
	class RayPlaneIntersection
	{
		public:
			RayPlaneIntersection() : altitude       ( 0 ),
			                         distanceSquared( 0 ),
			                         pointOnPlane   ( Point2D::Type() )  {}
			
			double altitude;         // distance from ray starting point to the plane
			double distanceSquared;  // distance from ray starting point to intersection
			Point2D::Type pointOnPlane;
	};
	
	
	RayPlaneIntersection IntersectionOfRayWithPlane3D( const Point3D::Type& inPtA,
	                                                   const Point3D::Type& inPtB,
	                                                   const XMatrix& inWorld2Plane );
	
	RayPlaneIntersection IntersectionOfRayWithPlane3D( const Point3D::Type& ptA,
	                                                   const Point3D::Type& ptB,
	                                                   const Plane3D::Type& plane );
	
}

#endif

