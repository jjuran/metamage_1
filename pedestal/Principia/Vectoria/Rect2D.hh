/*	==================
 *	Vectoria/Rect2D.hh
 *	==================
 */

#ifndef VECTORIA_RECT2D_HH
#define VECTORIA_RECT2D_HH

#include <algorithm>

#include "Vectoria/Coordinates.hh"


namespace Vectoria
{
	
	template < class Num >
	class Rect2D
	{
		public:
			Num left, bottom, right, top;
			
			typedef Num value_type;
			
			Rect2D() {}
			
			Rect2D( Num inLeft,
			        Num inBottom,
			        Num inRight,
			        Num inTop ) : left  ( inLeft   ),
			                      bottom( inBottom ),
			                      right ( inRight  ),
			                      top   ( inTop    )  {}
			
			bool IsEmpty() const  { return left >= right  ||  bottom >= top; }
			
			template < class Point >
			bool ContainsPoint( const Point& point ) const;
			
			template < class NumB >
			Rect2D< Num >& operator=( const Rect2D< NumB >& rect )
			{
				left   = Num( rect.left   );
				bottom = Num( rect.bottom );
				top    = Num( rect.top    );
				right  = Num( rect.right  );
				
				return *this;
			}
	};
	
	template < class Num >
	Rect2D< Num > operator*( const Rect2D< Num >& a,
	                         const Rect2D< Num >& b )
	{
		Rect2D< Num > r;
		
		r.left   = std::max( a.left,   b.left   );
		r.bottom = std::max( a.bottom, b.bottom );
		r.right  = std::min( a.right,  b.right  );
		r.top    = std::min( a.top,    b.top    );
		
		return r;
	}
	
	template < class Num >
	template < class Point >
	bool Rect2D< Num >::ContainsPoint( const Point& pt ) const
	{
		double x = pt[X];
		double y = pt[Y];
		
		return    x > left
		       && x < right
		       && y > bottom
		       && y < top;
	}
	
}

#endif

