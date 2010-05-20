/*	======================
 *	Vectoria/Clipping3D.hh
 *	======================
 */

#ifndef VECTORIA_CLIPPING3D_HH
#define VECTORIA_CLIPPING3D_HH

#include "Vectoria/LinearAlgebra3D.hh"


namespace Vectoria
{
	
	template < class Point, class Plane >
	inline bool ClipPointAgainstPlane( const Point& pt, const Plane& plane )
	{
		return DotProduct( plane, pt ) >= 0;
	}
	
	template < class Points, class Plane >
	void ClipPolygonAgainstPlane( Points& points, const Plane& plane )
	{
		if ( points.size() < 3 )
		{
			points.clear();
			return;
		}
		
		Points newPoints;
		
		typedef typename Points::const_iterator Iter;
		typedef typename Points::value_type     Point;
		
		bool havePrevious = false;
		bool previousIsPositive;
		
		// For each polygon vertex...
		for ( Iter it = points.begin(), end = points.end();  it != end;  ++it )
		{
			Point pt = *it;
			bool positive = ClipPointAgainstPlane( pt, plane );
			
			// Is this vertex on the other side of the clipping plane than the previous?
			// In other words, did we cross the clipping plane?
			// If this is the first vertex (ergo there's no previous), we didn't cross.
			bool crossed = havePrevious  &&  ( previousIsPositive != positive );
			
			// Every time we cross into negative space, we need exactly two vertices
			// on the clipping plane.
			// We add one when we cross in, one when we cross out.
			if ( positive )
			{
				// Positive side
				if ( crossed )
				{
					// Crossed out of negative space.
					// The previous vertex, clipped, completes our trip.
					Point prev = *( it - 1 );
					prev = LinePlaneIntersection( pt - prev, pt, plane );
					newPoints.push_back( prev );
				}
				
				// Add the unclipped vertex in positive space.
				newPoints.push_back( pt );
			}
			else if ( crossed )
			{
				// Crossed into negative space.
				// The current vertex, clipped, begins our trip.
				Point prev = *( it - 1 );
				pt = LinePlaneIntersection( pt - prev, pt, plane );
				newPoints.push_back( pt );
			}
			else
			{
				// Nothing to do in negative space.
			}
			
			previousIsPositive = positive;
			havePrevious = true;
		}
		
		Point firstPt = points.front();
		
		bool firstIsPositive = ClipPointAgainstPlane( firstPt, plane );
		
		if ( firstIsPositive != previousIsPositive )
		{
			// There is a crossing between the 'last' and 'first' vertices.
			// If the current vertex is on the negative side (code != 0),
			// Then that vertex is clipped and added.
			// Otherwise, the first vertex is clipped and added.
			Point pt = points.back();
			
			pt = LinePlaneIntersection( firstPt - pt, pt, plane );
			
			newPoints.push_back( pt );
		}
		
		using std::swap;
		
		swap( points, newPoints );
	}
	
}

#endif

