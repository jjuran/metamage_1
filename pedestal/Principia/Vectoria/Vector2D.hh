/*	====================
 *	Vectoria/Vector2D.hh
 *	====================
 */

#ifndef VECTORIA_VECTOR2D_HH
#define VECTORIA_VECTOR2D_HH

#if PRAGMA_ONCE
#pragma once
#endif

#include "Vectoria/Vector.hh"


namespace Vectoria
{
	
	struct Vector2D
	{
		typedef Vector< double, 3 >::Type Type;
		
		static Type Make( double dx,
		                  double dy )
		{
			Type vector;
			
			double* cells = vector.begin();
			
			cells[0] = dx;
			cells[1] = dy;
			cells[2] = 0.0;
			
			return vector;
		}
	};
	
}

#endif

