/*	==================
 *	Vectoria/Matrix.cc
 *	==================
 */

#include "Vectoria/Matrix.hh"


namespace Vectoria
{
	
	namespace CompileTests
	{
		
		typedef Matrix< double, 1, 1 > OneByOne;
		
		typedef Matrix< double, 2, 2 > TwoByTwo;
		
		typedef Matrix< double, 1, 4 > OneByFour;
		
		typedef Matrix< double, 4, 1 > FourByOne;
		typedef Matrix< double, 4, 4 > FourByFour;
		
		static OneByOne ID_1()
		{
			return IdentityMatrix::Make< double, 1 >();
		}
		
		static FourByFour ID_4()
		{
			return IdentityMatrix::Make< double, 4 >();
		}
		
		static FourByOne Zero_4x1()
		{
			return ZeroMatrix::Make< double, 4, 1 >();
		}
		
		static OneByFour Transpose_4x1( const FourByOne& m )
		{
			return Transpose( m );
		}
		
		static FourByOne Negative_4x1( const FourByOne& a )
		{
			return -a;
		}
		
		static FourByOne Plus_4x1( const FourByOne& a, const FourByOne& b )
		{
			return a + b;
		}
		
		static FourByOne Minus_4x1( const FourByOne& a, const FourByOne& b )
		{
			return a - b;
		}
		
		static FourByOne Times_scalar( const FourByOne& a, double f )
		{
			bool irrelevant = f > 0;
			
			return irrelevant ? a * f : f * a;
		}
		
		static FourByOne DividedBy_scalar( const FourByOne& a, double d )
		{
			return a / d;
		}
		
		static FourByFour Times_4x4x4( const FourByFour& a, const FourByFour& b )
		{
			return a * b;
		}
		
		static FourByOne Times_4x4x1( const FourByFour& a, const FourByOne& b )
		{
			return a * b;
		}
		
		static FourByOne Times_4x1x1( const FourByOne& a, const OneByOne& b )
		{
			return a * b;
		}
		
		static FourByFour Times_4x1x4( const FourByOne& a, const OneByFour& b )
		{
			return a * b;
		}
		
		static OneByOne Times_1x4x1( const OneByFour& a, const FourByOne& b )
		{
			return a * b;
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

