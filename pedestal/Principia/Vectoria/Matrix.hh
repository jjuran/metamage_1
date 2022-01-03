/*	==================
 *	Vectoria/Matrix.hh
 *	==================
 */

#ifndef VECTORIA_MATRIX_HH
#define VECTORIA_MATRIX_HH

#include <algorithm>


namespace Vectoria
{
	
	// Template parameters
	// -------------------
	// 
	// Component is the component type -- the type of the individual values of
	// which the matrix is composed, often float or double.  Another choice is
	// int, if you don't mind the division error (though it's useful for testing).
	// Any numeric type (possibly user-defined) is fine.
	// We also use T instead of Component for brevity.
	// 
	// R is the number of rows.
	// C is the number of columns.
	// 
	// In matrix multiplication (A * B):
	// * R1 is the row count of A (and the result)
	// * C2 is the column count of B (and the result)
	// * R2C1 is the row count of B and the column count of A (which must match)
	// 
	// Len is the length (i.e. number of rows/columns) of a square matrix.
	
	template < class Component, unsigned R, unsigned C >
	class Matrix
	{
		public:
			static const unsigned rowCount    = R;
			static const unsigned columnCount = C;
			static const unsigned cellCount   = rowCount * columnCount;
			
			typedef Matrix< Component, R, C > This;
			typedef Component value_type;
			typedef Component Array[ rowCount * columnCount ];
		
		private:
			Array myCells;
		
		public:
			static unsigned RowCount()     { return rowCount;    }
			static unsigned ColumnCount()  { return columnCount; }
			static unsigned Size()         { return cellCount;   }
			
			// Constructor
			Matrix()  {}
			
			template < class Initializer >
			Matrix( Initializer init )
			{
				init.Initialize( *this );
			}
			
			static unsigned size()        { return rowCount * columnCount;   }
			
			Component const* data() const  { return myCells; }
			Component      * data()        { return myCells; }
			
			typedef const Component* const_iterator;
			typedef       Component*       iterator;
			
			const_iterator begin() const  { return myCells;          }
			      iterator begin()        { return myCells;          }
			const_iterator end() const    { return myCells + size(); }
			      iterator end()          { return myCells + size(); }
			
			template < class Coordinate >  Component const& operator[]( Coordinate c ) const  { return c( *this ); }
			template < class Coordinate >  Component      & operator[]( Coordinate c )        { return c( *this ); }
		
		public:
			// Get/Set
			Component const& Cell( unsigned row, unsigned col ) const  { return myCells[ row * columnCount + col ]; }
			Component      & Cell( unsigned row, unsigned col )        { return myCells[ row * columnCount + col ]; }
			
			Component const& operator()( unsigned row, unsigned col ) const  { return myCells[ row * columnCount + col ]; }
			Component      & operator()( unsigned row, unsigned col )        { return myCells[ row * columnCount + col ]; }
			
			// Arithmetic operators
			This& operator+=( const This& other );
			This& operator-=( const This& other );
			
			This& invert_each_element();
			
			This& operator*=( Component factor  );
			This& operator/=( Component divisor );
			
			// Factor must be a square matrix with dimensions == our width
			This& operator*=( const Matrix< Component, C, C >& factor )
			{
				return *this = *this * factor;
			}
			
			bool operator==( const This& other )
			{
				return std::equal( begin(), end(), other.begin() );
			}
	};
	
	template < class T, unsigned R, unsigned C >
	Matrix< T, R, C >& Matrix< T, R, C >::operator+=( const Matrix< T, R, C >& x )
	{
		value_type const* src = x.myCells;
		
		value_type* dst = myCells;
		value_type* end = myCells + cellCount;
		
		while ( dst < end )
		{
			*dst++ += *src++;
		}
		
		return *this;
	}
	
	template < class T, unsigned R, unsigned C >
	Matrix< T, R, C >& Matrix< T, R, C >::operator-=( const Matrix< T, R, C >& x )
	{
		value_type const* src = x.myCells;
		
		value_type* dst = myCells;
		value_type* end = myCells + cellCount;
		
		while ( dst < end )
		{
			*dst++ -= *src++;
		}
		
		return *this;
	}
	
	template < class T, unsigned R, unsigned C >
	Matrix< T, R, C >& Matrix< T, R, C >::invert_each_element()
	{
		value_type* dst = myCells;
		value_type* end = myCells + cellCount;
		
		while ( dst < end )
		{
			value_type v = *dst;
			
			*dst++ = -v;
		}
		
		return *this;
	}
	
	template < class T, unsigned R, unsigned C >
	Matrix< T, R, C >& Matrix< T, R, C >::operator*=( value_type factor )
	{
		value_type* dst = myCells;
		value_type* end = myCells + cellCount;
		
		while ( dst < end )
		{
			*dst++ *= factor;
		}
		
		return *this;
	}
	
	template < class T, unsigned R, unsigned C >
	Matrix< T, R, C >& Matrix< T, R, C >::operator/=( value_type factor )
	{
		value_type* dst = myCells;
		value_type* end = myCells + cellCount;
		
		while ( dst < end )
		{
			*dst++ /= factor;
		}
		
		return *this;
	}
	
	
	template < class T, unsigned R, unsigned C >
	Matrix< T, C, R > Transpose( const Matrix< T, R, C >& matrix )
	{
		Matrix< T, C, R > transpose;
		
		for ( unsigned x = 0;  x < R;  ++x )
		{
			for ( unsigned y = 0;  y < C;  ++y )
			{
				transpose.Cell( y, x ) = matrix.Cell( x, y );
			}
		}
		
		return transpose;
	}
	
	template < class T, unsigned R, unsigned C >
	Matrix< T, R, C > operator-( const Matrix< T, R, C >& matrix )
	{
		Matrix< T, R, C > copy = matrix;
		
		copy.invert_each_element();
		
		return copy;
	}
	
	template < class T, unsigned R, unsigned C >
	inline Matrix< T, R, C > operator+( const Matrix< T, R, C >& a,
	                                    const Matrix< T, R, C >& b )
	{
		Matrix< T, R, C > sum = a;
		
		sum += b;
		
		return sum;
	}
	
	template < class T, unsigned R, unsigned C >
	inline Matrix< T, R, C > operator-( const Matrix< T, R, C >& a,
	                                    const Matrix< T, R, C >& b )
	{
		Matrix< T, R, C > difference = a;
		
		difference -= b;
		
		return difference;
	}
	
	template < class T, unsigned R, unsigned C >
	inline Matrix< T, R, C > operator*( const Matrix< T, R, C >&  matrix,
	                                    T                         factor )
	{
		Matrix< T, R, C > product = matrix;
		
		product *= factor;
		
		return product;
	}
	
	template < class T, unsigned R, unsigned C >
	inline Matrix< T, R, C > operator*( T                         factor,
	                                    const Matrix< T, R, C >&  matrix )
	{
		return matrix * factor;
	}
	
	template < class T, unsigned R, unsigned C >
	inline Matrix< T, R, C > operator/( const Matrix< T, R, C >&  matrix,
	                                    T                         divisor )
	{
		Matrix< T, R, C > quotient = matrix;
		
		quotient /= divisor;
		
		return quotient;
	}
	
	template < class T, unsigned R1, unsigned R2C1, unsigned C2 >
	Matrix< T, R1, C2 > operator*( const Matrix< T, R1, R2C1 >&  a,
	                               const Matrix< T, R2C1, C2 >&  b )
	{
		Matrix< T, R1, C2 > result;
		
		T* data = result.begin();
		
		for ( unsigned i = 0;  i < R1;  ++i )
		{
			for ( unsigned j = 0;  j < C2;  ++j )
			{
				unsigned cell = i * C2 + j;
				data[ cell ] = 0;
				
				for ( unsigned k = 0;  k < R2C1;  k++ )
				{
					data[ cell ] += a.Cell( i, k ) * b.Cell( k, j );
				}
			}
		}
		
		return result;
	}
	
	
	struct ZeroMatrix
	{
		typedef ZeroMatrix This;
		
		template < class T, unsigned R, unsigned C >
		static Matrix< T, R, C >&  Initialize( Matrix< T, R, C >& matrix );
		
		template < class T, unsigned R, unsigned C >
		static Matrix< T, R, C >   Make()  { return Matrix< T, R, C >( This() ); }
	};
	
	template < class T, unsigned R, unsigned C >
	inline Matrix< T, R, C >&
	ZeroMatrix::Initialize( Matrix< T, R, C >& matrix )
	{
		std::fill( matrix.begin(), matrix.end(), T( 0 ) );
		
		return matrix;
	}
	
	
	struct IdentityMatrix
	{
		typedef IdentityMatrix This;
		
		template < class T, unsigned Len >
		static Matrix< T, Len, Len >&  Initialize( Matrix< T, Len, Len >& matrix );
		
		template < class T, unsigned Len >
		static Matrix< T, Len, Len >   Make()  { return Matrix< T, Len, Len >( This() ); }
	};
	
	template < class T, unsigned Len >
	Matrix< T, Len, Len >&
	IdentityMatrix::Initialize( Matrix< T, Len, Len >& matrix )
	{
		ZeroMatrix::Initialize( matrix );
		
		for ( unsigned i = 0;  i < Len;  ++i )
		{
			matrix.Cell( i, i ) = 1;
		}
		
		return matrix;
	}
	
}

#endif
