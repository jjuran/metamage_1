/*	===========================
 *	Vectoria/LinearAlgebra3D.cc
 *	===========================
 */

#include "Vectoria/LinearAlgebra3D.hh"

// Vertice
#include "Vectoria/PairOfPoints2D.hh"
#include "Vectoria/LinearAlgebra2D.hh"
#include "Vectoria/Vector.hh"
#include "Vectoria/Vector3D.hh"


namespace Vectoria
{
	
	RayPlaneIntersection IntersectionOfRayWithPlane3D( const Point3D::Type& inPtA,
	                                                   const Point3D::Type& inPtB,
	                                                   const XMatrix& world2Plane )
	{
		Point3D::Type ptA = inPtA;
		Point3D::Type ptB = inPtB;
		
		ptA = Transformation( ptA, world2Plane );
		ptB = Transformation( ptB, world2Plane );
		
		// Points A and B are now in plane coordinates.
		if ( ptA[ Z ] <= 0  ||  ptA[ Z ] - ptB[ Z ] <= 0 )
		{
			// The plane is behind the ray's starting point, 
			// or the ray is parallel to the plane; no intersection
			// Return z == 0 to indicate no intersection.
			return RayPlaneIntersection();
		}
		
		double x = YIntercept( PairOfPoints2D( Point2D::Make( ptA[ Z ], ptA[ X ] ),
		                                       Point2D::Make( ptB[ Z ], ptB[ X ] ) ) );
		
		double y = YIntercept( PairOfPoints2D( Point2D::Make( ptA[ Z ], ptA[ Y ] ),
		                                       Point2D::Make( ptB[ Z ], ptB[ Y ] ) ) );
		
		RayPlaneIntersection intersection;
		intersection.pointOnPlane = Point2D::Make( x, y );
		
		Point3D::Type  pt   = Point3D::Make( x, y, 0 );
		Vector3D::Type dist = pt - ptA;
		
		intersection.distanceSquared = MagnitudeSquared( dist );
		intersection.altitude = ptA[ Z ];
		
		return intersection;
	}
	
	RayPlaneIntersection IntersectionOfRayWithPlane3D( const Point3D::Type& ptA,
	                                                   const Point3D::Type& ptB,
	                                                   const Plane3D::Type& plane )
	{
		RayPlaneIntersection intersection;
		
		intersection.altitude = DotProduct( plane, ptA );
		
		if ( intersection.altitude <= 0 )
		{
			// Pt A is in or behind the plane, so it's a backface.  Cull it.
			// Return z == 0 to indicate no intersection.
			return RayPlaneIntersection();
		}
		
		double distance = -intersection.altitude / DotProduct( plane, ptB - ptA );
		
		if ( distance <= 0 )
		{
			// The intersection is behind the ray's starting point,
			// so the plane is behind the camera and offscreen.
			// Return z == 0 to indicate no intersection.
			return RayPlaneIntersection();
		}
		
		return intersection;
	}
	
}

