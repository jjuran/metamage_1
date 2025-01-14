/*
	pinned.hh
	---------
*/

#ifndef FREND_PINNED_HH
#define FREND_PINNED_HH


namespace frend
{

template < class Num >
inline
Num pinned( Num x, Num min, Num max )
{
	return + x < min ? min
	       : x > max ? max
	       : x;
}

template < class Num >
inline
Num pinned( Num x, Num max )
{
	return pinned( x, Num(), max );
}

/*
	This could be generalized to pin to an arbitrary
	rectangle, but there isn't currently a need.
*/

template < class Point >
Point pinned_location( Point location, Point limit )
{
	location.x = pinned( location.x, limit.x );
	location.y = pinned( location.y, limit.y );
	
	return location;
}

}

#endif
