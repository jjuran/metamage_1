/*	=================
 *	CrossProduct3D.hh
 *	=================
 */

#ifndef VECTORIA_CROSSPRODUCT_HH
#define VECTORIA_CROSSPRODUCT_HH

#if PRAGMA_ONCE
#pragma once
#endif

// Vertice
#include "Vectoria/Coordinates.hh"
#include "Vectoria/Vector.hh"
#include "Vectoria/Vector3D.hh"


namespace Vectoria
{
	
	template < class Vector_A, class Vector_B >
	inline Vector3D::Type CrossProduct( const Vector_A& a, const Vector_B& b )
	{
		double dx = a[ Y ] * b[ Z ] - a[ Z ] * b[ Y ];
		double dy = a[ Z ] * b[ X ] - a[ X ] * b[ Z ];
		double dz = a[ X ] * b[ Y ] - a[ Y ] * b[ X ];
		
		return Vector3D::Make( dx, dy, dz );
	}
	
}

#endif

