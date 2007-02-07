/*	=====================
 *	Vectoria/Polygon3D.hh
 *	=====================
 */

#ifndef VECTORIA_POLYGON3D_HH
#define VECTORIA_POLYGON3D_HH

// Standard C++
#include <algorithm>
#include <functional>
#include <vector>

// Vertice
#include "Vectoria/Transform.hh"
#include "Vectoria/CrossProduct3D.hh"
#include "Vectoria/Plane3D.hh"
#include "Vectoria/Polygon2D.hh"
#include "Vectoria/Point3D.hh"
#include "Vectoria/Rect3D.hh"
#include "Vectoria/Vector.hh"


namespace Vectoria
{
	
	class Polygon3D
	{
		public:
			typedef Point3D::Type Point;
		
		private:
			std::vector< Point > myPoints;
		
		public:
			Polygon3D()  {}
			
			Polygon3D( const std::vector< Point >& points ) : myPoints( points )  {}
			
			Polygon3D( const Polygon2D& poly2d );
			
			const std::vector< Point >& Points() const  { return myPoints; }
	};
	
	
	inline Point3D::Type Point3DFrom2D( const Point2D::Type& point )
	{
		return Point3D::Make( point[X], point[Y], 0.0 );
	}
	
	inline Polygon3D::Polygon3D( const Polygon2D& poly2d ) : myPoints( poly2d.Points().size() )
	{
		const std::vector< Point2D::Type >& pts2d = poly2d.Points();
		
		std::transform( pts2d.begin(),
		                pts2d.end(),
		                myPoints.begin(),
		                std::ptr_fun( Point3DFrom2D ) );
	}
	
	template < class Points >
	inline Vector3D::Type LeadingTangent( const Points& points )
	{
		return points[ 1 ] - points[ 0 ];
	}
	
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
	
	template < class Points >
	inline Plane3D::Type PlaneVector( const Points& points )
	{
		Vector3D::Type normal = FaceNormal( points );
		
		double D = ProportionalDistanceFromOrigin( normal, points[ 0 ] );
		
		return Plane3D::Make( normal, D );
	}
	
	template < class Points >
	inline Point2D::Type ProjectionOfPointOntoPolygon( const Points& points, const Point3D::Type& pt )
	{
		Vector3D::Type v = pt - points[ 0 ];
		Vector3D::Type tangent = LeadingTangent( points );
		
		double x = Magnitude( Projection   ( v, tangent ) );
		double y = Magnitude( Perpendicular( v, tangent ) );
		
		return Point2D::Make( x, y );
	}
	
	template < class Points >
	Rect3D BoundingRect( const Points& points )
	{
		std::vector< Point3D::Type >::const_iterator it = points.begin(), done = points.end();
		Point3D::Type pt = *it;
		
		double left   = pt[ X ];
		double bottom = pt[ Y ];
		double front  = pt[ Z ];
		double right  = left;
		double top    = bottom;
		double back   = front;
		
		for ( ++it;  it != done;  ++it )
		{
			pt = *it;
			
			double x = pt[ X ];
			
			if ( x < left )
			{
				left = x;
			}
			else if ( x > right )
			{
				right = x;
			}
			
			double y = pt[ Y ];
			
			if ( y < bottom )
			{
				bottom = y;
			}
			else if ( y > top )
			{
				top = y;
			}
			
			double z = pt[ Z ];
			
			if ( z < back )
			{
				back = z;
			}
			else if ( z > front )
			{
				front = z;
			}
		}
		
		return Rect3D( left, bottom, back, right, top, front );
	}
	
	inline Polygon3D operator*( const XMatrix& plotter, const Polygon3D& poly )
	{
		const std::vector< Point3D::Type >& pts = poly.Points();
		
		std::vector< Point3D::Type > newPts( pts.size() );
		
		std::transform( pts.begin(),
		                pts.end(),
		                newPts.begin(),
		                Transformer< Point3D::Type >( plotter ) );
		
		return newPts;
	}
	
}

#endif

