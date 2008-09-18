/*	===================
 *	Vectoria/Point2D.hh
 *	===================
 */

#ifndef VECTORIA_POINT2D_HH
#define VECTORIA_POINT2D_HH

#if PRAGMA_ONCE
#pragma once
#endif

#include "Vectoria/Vector.hh"


namespace Vectoria
{
	
	struct Point2D
	{
		typedef Vector< double, 3 >::Type Type;
		
		static Type Make( double dx,
		                  double dy )
		{
			Type vector;
			
			double* cells = vector.begin();
			
			cells[0] = dx;
			cells[1] = dy;
			cells[2] = 1.0;
			
			return vector;
		}
	};
	
}

#endif

