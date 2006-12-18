/*	=====================
 *	Vectoria/Polygon2D.hh
 *	=====================
 */

#ifndef VECTORIA_POLYGON2D_HH
#define VECTORIA_POLYGON2D_HH

// Standard C++
#include <vector>

// Vertice
#include "Vectoria/Point2D.hh"
#include "Vectoria/PairOfPoints2D.hh"
#include "Vectoria/LinearAlgebra2D.hh"
#include "Vectoria/Rect2D.hh"


namespace Vectoria
{
	
	class Polygon2D
	{
		public:
			typedef double value_type;
			typedef Point2D::Type Point;
			typedef PairOfPoints2D edge_type;
			
		private:
			std::vector< Point > myPoints;
		
		public:
			Polygon2D() {}
			
			Polygon2D( const Rect2D< double >& r )
			{
				myPoints.push_back( Point2D::Make( r.left,  r.bottom ) );
				myPoints.push_back( Point2D::Make( r.left,  r.top    ) );
				myPoints.push_back( Point2D::Make( r.right, r.top    ) );
				myPoints.push_back( Point2D::Make( r.right, r.bottom ) );
			}
			
			const std::vector< Point >& Points() const  { return myPoints; }
			      std::vector< Point >& Points()        { return myPoints; }
			
			static edge_type MakeEdge( const Point& ptA, const Point& ptB )
			{
				return edge_type( ptA, ptB );
			}
			
			std::vector< edge_type > Edges() const
			{
				const std::vector< Point >& pts0 = Points();
				
				std::vector< Point > pts1( pts0.size() );
				
				std::rotate_copy( pts0.begin(),
				                  pts0.begin() + 1,
				                  pts0.end(),
				                  pts1.begin() );
				
				std::vector< edge_type > edges( pts0.size() );
				
				std::transform( pts0.begin(),
				                pts0.end(),
				                pts1.begin(),
				                edges.begin(),
				                MakeEdge );
				
				return edges;
			}
			
			Rect2D< double > BoundingRect() const;
			
			bool ContainsPoint( const Point& point ) const;
	};
	
	inline Rect2D< double > Polygon2D::BoundingRect() const
	{
		std::vector< Point >::const_iterator it = myPoints.begin(), done = myPoints.end();
		
		const Point& first( *it );
		
		double left   = first[X];
		double bottom = first[Y];
		double right  = left;
		double top    = bottom;
		
		for ( ++it;  it != done;  ++it )
		{
			const Point& pt( *it );
			
			double x = pt[X];
			
			if ( x < left )
			{
				left = x;
			}
			else if ( x > right )
			{
				right = x;
			}
			
			double y = pt[Y];
			
			if ( y < bottom )
			{
				bottom = y;
			}
			else if ( y > top )
			{
				top = y;
			}
		}
		
		return Rect2D< double >( left, bottom, right, top );
	}
	
	inline bool Polygon2D::ContainsPoint( const Point& point ) const
	{
		// Compute the bounding rectangle.
		Rect2D< double > rect = BoundingRect();
		
		// If the point lies outside the bounding rect, it's outside the polygon.
		if ( !rect.ContainsPoint( point ) )
		{
			return false;
		}
		
		// We need to construct a point outside the polygon.
		// Any point outside the bounding rectangle will suffice.
		// Any point left of the rectangle (i.e. whose x < left) is outside it.
		// The y coordinate is therefore arbitrary.  Matching the y of the
		// point in question yields a horizontal anchor line.
		Point outside = Point2D::Make( rect.left - 1, point[Y] );
		
		// The anchor is a hypothetical line segment from some point outside
		// the polygon to the point in question.
		edge_type anchor( outside, point );
		
		// We'll keep track of how many times the anchor crosses the polygon's edges.
		int crossings = 0;
		
		const std::vector< edge_type >& edges = Edges();
		std::vector< edge_type >::const_iterator it, done = edges.end();
		
		for ( it = edges.begin();  it != done;  ++it )
		{
			if ( LineSegmentsIntersect( anchor, *it ) )
			{
				crossings++;
			}
		}
		
		// Return whether the number of crossings is odd, which is also whether
		// the polygon contains the point.
		return crossings % 2 == 1;
	}
	
}

#endif

