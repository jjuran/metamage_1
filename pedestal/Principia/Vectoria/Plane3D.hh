/*	===================
 *	Vectoria/Plane3D.hh
 *	===================
 */

#ifndef VECTORIA_PLANE3D_HH
#define VECTORIA_PLANE3D_HH

#include "Vectoria/Coordinates.hh"
#include "Vectoria/Vector.hh"


namespace Vectoria
{
	
	// Three-dimensional plane in 4-space.
	
	struct Plane3D
	{
		typedef Vector< double, 4 >::Type Type;
		
		static Type Make( double dx,
		                  double dy,
		                  double dz,
		                  double D )
		{
			Type vector;
			
			double* cells = vector.begin();
			
			cells[0] = dx;
			cells[1] = dy;
			cells[2] = dz;
			cells[3] = D;
			
			return vector;
		}
		
		template < class Normal >
		static Type Make( const Normal& N, double D )
		{
			return Make( N[X], N[Y], N[Z], D );
		}
	};
	
}

#endif

