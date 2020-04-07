/*	================
 *	Vectoria/Trig.hh
 *	================
 */

#ifndef VECTORIA_TRIG_HH
#define VECTORIA_TRIG_HH

// Standard C/C++
#include <cmath>


namespace Vectoria
{
	
	const double piOver2 = std::asin( 1.0 );
	
	const double pi = piOver2 * 2;
	
	template < class Num, class Angle >
	inline void Polar2XY( Num    r,
	                      Angle  theta,
	                      Num&   x,
	                      Num&   y )
	{
		x = r * std::cos( theta );
		y = r * std::sin( theta );
	}
	
	template < class Num, class Angle >
	inline void XY2Polar( Num     x,
	                      Num     y,
	                      Num&    r,
	                      Angle&  theta )
	{
		theta = (x == 0) ? ( (y >= 0) ?  pi/2
		                              : -pi/2 )
		                 : std::atan( y/x ) + ( (x < 0) ? pi
		                                           : 0 );
		
		r = (x == 0) ? ( y / std::sin( theta ) )
		             : ( x / std::cos( theta ) );
	}
}


/*
template <class Point>
int ComparePointToLineUsingPolar(Point pt, Point a, Point b)
{
	if (pt == a) return 0;
	
	Point::vector_type a2b  = b  - a;
	Point::vector_type a2pt = pt - a;
	
	typedef Polar2D<Point::value_type> Polar;
	
	Polar::angle_type angle = XY2Polar(a2pt).Theta() - XY2Polar(a2b).Theta();
	//Num sine = sin(angle);
	
	return sign(sin(angle));
}
*/

#endif
