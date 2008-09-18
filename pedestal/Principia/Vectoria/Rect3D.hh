/*	==================
 *	Vectoria/Rect3D.hh
 *	==================
 */

#ifndef VECTORIA_RECT3D_HH
#define VECTORIA_RECT3D_HH

#include "Vectoria/Coordinates.hh"


namespace Vectoria
{
	
	class Rect3D
	{
		public:
			typedef double value_type;
			
			Rect3D() {}
			
			Rect3D( double left,
			        double bottom,
			        double back,
			        double right,
			        double top,
			        double front ) : left  ( left   ),
			                         bottom( bottom ),
			                         back  ( back   ),
			                         right ( right  ),
			                         top   ( top    ),
			                         front ( front  )  {}
			
			bool IsEmpty() const  { return left >= right  ||  bottom >= top  ||  back >= front; }
			
			template < class Point >
			bool ContainsPoint( const Point& point ) const;
		
		//private:
			double left, bottom, back, right, top, front;
	};
	
	template < class Point >
	inline bool Rect3D::ContainsPoint( const Point& point ) const
	{
		double x = point[ X ];
		double y = point[ Y ];
		double z = point[ Z ];
		
		return x > left   && x < right
			&& y > bottom && y < top
			&& z > back   && y < front;
	}
	
}

#endif

