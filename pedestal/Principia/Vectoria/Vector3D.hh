/*	====================
 *	Vectoria/Vector3D.hh
 *	====================
 */

#ifndef VECTORIA_VECTOR3D_HH
#define VECTORIA_VECTOR3D_HH

#if PRAGMA_ONCE
#pragma once
#endif

#include "Vectoria/Vector.hh"


namespace Vectoria
{
	
	struct Vector3D
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
			cells[3] = 0.0;
			
			return vector;
		}
	};
	
}

#endif

