/*	==================
 *	Vectoria/Vector.cc
 *	==================
 */

#include "Vectoria/Vector.hh"


namespace Vectoria
{
	
	namespace CompileTests
	{
		
		typedef Vector< double, 4 > V4;
		
		typedef V4::Type Vector_4;
		
		static unsigned VectorLength( const Vector_4& v )
		{
			return Dimensionality( v );
		}
		
		static double DotProduct_4( const Vector_4& a, const Vector_4& b )
		{
			return DotProduct( a, b );
		}
		
		static double Magnitude_4( const Vector_4& v )
		{
			return Magnitude( v );
		}
		
		static Vector_4 UnitLength_4( const Vector_4& v )
		{
			return UnitLength( v );
		}
		
		static Vector_4 Projection_4( const Vector_4& p, const Vector_4& q )
		{
			return Projection( p, q );
		}
		
		static Vector_4 Perpendicular_4( const Vector_4& p, const Vector_4& q )
		{
			return Perpendicular( p, q );
		}
		
	}
	
}

#ifdef __APPLE__

namespace _
{
	
	void dummy()
	{
	}
	
}

#endif

