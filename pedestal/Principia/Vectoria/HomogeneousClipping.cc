/*	===============================
 *	Vectoria/HomogeneousClipping.cc
 *	===============================
 */

#include "Vectoria/HomogeneousClipping.hh"


namespace Vectoria
{
	
	Matrix< double, 4, 4 > MakePortToClipTransform( double  near_distance,
	                                                double  far_distance,
	                                                double  focal_length,
	                                                double  aspect_ratio )
	{
		Matrix< double, 4, 4 > result = ZeroMatrix();
		
		double n = near_distance;
		double f = far_distance;
		double e = focal_length;
		double a = aspect_ratio;
		
		double l = -n / e;
		double r = -l;
		double b = a * l;
		double t = -b;
		
		result.Cell( 0, 0 ) =    2*n   / (r - l);
		// 0
		result.Cell( 0, 2 ) =  (r + l) / (r - l);
		// 0
		
		// 0
		result.Cell( 1, 1 ) =    2*n   / (t - b);
		result.Cell( 1, 2 ) =  (t + b) / (t - b);
		// 0
		
		// 0
		// 0
		
		#if 1
		
		result.Cell( 2, 2 ) = -(f + n) / (f - n);
		result.Cell( 2, 3 ) =  -2*n*f  / (f - n);
		
		#else
		
		result.Cell( 2, 2 ) =  (f + n) / (f - n);
		result.Cell( 2, 3 ) =   2*n*f  / (f - n);
		
		#endif
		
		// 0
		// 0
		result.Cell( 3, 2 ) =         -1;
		// 0
		
		return result;
	}
	
}

