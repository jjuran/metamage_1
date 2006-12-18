/*	===================
 *	Vectoria/Point3D.hh
 *	===================
 */

#ifndef VECTORIA_POINT3D_HH
#define VECTORIA_POINT3D_HH

#include "Vectoria/Vector.hh"


namespace Vectoria
{
	
	struct Point3D
	{
		typedef Vector< double, 4 >::Type Type;
		
		static Type Make( double dx,
		                  double dy,
		                  double dz )
		{
			Type vector;
			
			double* cells = vector.begin();
			
			cells[0] = dx;
			cells[1] = dy;
			cells[2] = dz;
			cells[3] = 1.0;
			
			return vector;
		}
	};
	
}

#endif

