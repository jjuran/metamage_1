/*	===============================
 *	Vectoria/HomogeneousClipping.hh
 *	===============================
 */

#ifndef VECTORIA_HOMOGENEOUSCLIPPING_HH
#define VECTORIA_HOMOGENEOUSCLIPPING_HH

#if PRAGMA_ONCE
#pragma once
#endif

#include "Vectoria/Matrix.hh"


namespace Vectoria
{
	
	Matrix< double, 4, 4 > MakePortToClipTransform( double  near_distance,
	                                                double  far_distance,
	                                                double  focal_length,
	                                                double  aspect_ratio );
	
}

#endif

