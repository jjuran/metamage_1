/*	===========================
 *	Vectoria/LinearAlgebra2D.hh
 *	===========================
 */

#ifndef VECTORIA_LINEARALGEBRA2D_HH
#define VECTORIA_LINEARALGEBRA2D_HH

// Standard C/C++
#include <cmath>

// Vertice
#include "Vectoria/Trig.hh"
#include "Vectoria/PairOfPoints2D.hh"


namespace Vectoria
{
	
	/*
	template < class Num >
	Num DistanceSquared2D( Num dx, Num dy )
	{
		return dx*dx + dy*dy;
	}
	
	template < class Num >
	Num Distance2D( Num dx, Num dy )
	{
		return std::sqrt( DistanceSquared2D( dx, dy ) );
	}
	
	inline double DistanceSquared( const Vector2D& vec )
	{
		return DistanceSquared2D( vec.dx, vec.dy );
	}
	
	inline double Distance( const Vector2D& vec )
	{
		return Distance2D( vec.dx, vec.dy );
	}
	*/
	
	// FIXME:  These two functions belong elsewhere.
	template < class Num >
	int Compare( Num a, Num b )
	{
		return (a < b) ? -1 :
		       (b < a) ?  1 : 0;
	}
	
	template < class Num >
	int sign( Num inNum )
	{
		return Compare( inNum, 0.0 );
	}
	
	class geometry_error {};
	
	inline double Rise( const PairOfPoints2D& inLine )
	{
		return Y2( inLine ) - Y1( inLine );
	}
	
	inline double Run( const PairOfPoints2D& inLine )
	{
		return X2( inLine ) - X1( inLine );
	}
	
	inline double Slope( const PairOfPoints2D& inLine )
	{
		if ( Run( inLine ) == 0 )
		{
			throw geometry_error();
		}
		
		return Rise( inLine ) / Run( inLine );
	}
	
	inline double YInterceptWithPrecalculatedSlope( const PairOfPoints2D& line, double m )
	{
		double x = X1( line );
		double y = Y1( line );
		
		double b = y - m * x;
		
		return b;
	}
	
	inline double YIntercept( const PairOfPoints2D& line )
	{
		double m = Slope( line );  // Will throw if x1 == x2
		
		return YInterceptWithPrecalculatedSlope( line, m );
	}
	
	inline double FofX( const PairOfPoints2D& line, double x )
	{
		double m = Slope( line );  // Will throw if x1 == x2
		double b = YInterceptWithPrecalculatedSlope( line, m );
		
		double y = m * x + b;
		
		return y;
	}
	
	template < class Point >
	inline int ComparePointToLine( const Point& pt, const PairOfPoints2D& line )
	{
		if ( X1( line ) == X2( line ) )
		{
			// Vertical line
			return Compare( pt[X], X1( line ) ) * sign( -Rise( line ) );
		}
		else
		{
			return Compare( pt[Y], FofX( line, pt[X] ) ) * sign( Run( line ) );
		}
	}
	
	inline bool LineSegmentsIntersect( const PairOfPoints2D& lineSegP,
	                                   const PairOfPoints2D& lineSegQ )
	{
		return       ComparePointToLine( PtA( lineSegP ), lineSegQ ) 
		          != ComparePointToLine( PtB( lineSegP ), lineSegQ )
		       &&    ComparePointToLine( PtA( lineSegQ ), lineSegP )
		          != ComparePointToLine( PtB( lineSegQ ), lineSegP );
	}
	
	/*
	template <class Num, class Line>
	Point2D<Num> LinesIntersection2D(const Line& a, const Line& b)
	{
		if (Run(a) == 0) {
			if (Run(b) == 0) {
				// Lines are both vertical -- no intersection
				throw geometry_error();
			} else {
				return FofX(b, X1(a));
			}
		} else if (Run(b) == 0) {
			return FofX(a, X1(b));
		}
		
		// Both lines are non-vertical.
		Num mA = Slope(a);
		Num mB = Slope(b);
		if (m == n) {
			// Slopes are the same -- no intersection
			throw geometry_error();
		}
		Num kA = YIntercept(a);
		Num kB = YIntercept(b);
		Num x = (k - b)/(m - n);
		Num y = FofX(a, x);
		return y;
	}
	*/
	
}

#endif

