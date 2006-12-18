/*	==========================
 *	Vectoria/PairOfPoints2D.hh
 *	==========================
 */

#ifndef VECTORIA_PAIROFPOINTS2D_HH
#define VECTORIA_PAIROFPOINTS2D_HH

// Vertice
#include "Vectoria/Coordinates.hh"
#include "Vectoria/Point2D.hh"


namespace Vectoria
{
	
	class PairOfPoints2D
	{
		public:
			typedef Point2D::Type point_type;
			
			point_type ptA;
			point_type ptB;
			
			PairOfPoints2D() {}
			PairOfPoints2D( const point_type& inA, const point_type& inB)
			:
				ptA( inA ),
				ptB( inB )
			{}
	};
	
	inline Point2D::Type PtA( const PairOfPoints2D& inLine )
	{
		return inLine.ptA;
	}
	
	inline Point2D::Type PtB( const PairOfPoints2D& inLine )
	{
		return inLine.ptB;
	}
	
	inline double X1( const PairOfPoints2D& inLine )
	{
		return PtA( inLine )[X];
	}
	
	inline double X2( const PairOfPoints2D& inLine )
	{
		return PtB( inLine )[X];
	}
	
	inline double Y1( const PairOfPoints2D& inLine )
	{
		return PtA( inLine )[Y];
	}
	
	inline double Y2( const PairOfPoints2D& inLine )
	{
		return PtB( inLine )[Y];
	}
	
}

#endif

