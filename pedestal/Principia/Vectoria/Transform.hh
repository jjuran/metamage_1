/*	=====================
 *	Vectoria/Transform.hh
 *	=====================
 */

#ifndef VECTORIA_TRANSFORM_HH
#define VECTORIA_TRANSFORM_HH

#include "Vectoria/Coordinates.hh"
#include "Vectoria/Matrix.hh"
#include "Vectoria/Units.hh"


namespace Vectoria
{
	
	typedef Matrix< double, 4, 4 > XMatrix;
	
	
	template < class Source, class Transform >
	Source Transformation( const Source& source, const Transform& xform )
	{
		return xform * source;
	}
	
	template < class Factor >
	class Transformer
	{
		private:
			XMatrix myTransform;
		
		public:
			Transformer( const XMatrix& inTransform ) : myTransform( inTransform )  {}
			
			Factor operator()( const Factor& factor ) const
			{
				return Transformation( factor, myTransform );
			}
	};
	
	inline XMatrix Compose( const XMatrix& f, const XMatrix& g )
	{
		return g * f;
	}
	
	inline XMatrix Compose( const XMatrix& f, const XMatrix& g, const XMatrix& h )
	{
		return h * g * f;
	}
	
	template < class Num >
	class Translation_T
	{
		private:
			Num dx, dy, dz;
		
		public:
			Translation_T() {}
			Translation_T( Num inDx, Num inDy, Num inDz ) : dx(inDx), dy(inDy), dz(inDz) {}
			
			Translation_T< Num > Inverse() const  { return Translation_T< Num >( -dx, -dy, -dz ); }
			
			Matrix< Num, 4, 4 > Make() const;
	};
	
	typedef Translation_T< double > Translation;
	
	template < class Num >
	Matrix< Num, 4, 4 > Translation_T< Num >::Make() const
	{
		Matrix< Num, 4, 4 > matrix = IdentityMatrix::Make< Num, 4 >();
		
		matrix.Cell( 0, 3 ) = dx;
		matrix.Cell( 1, 3 ) = dy;
		matrix.Cell( 2, 3 ) = dz;
		
		return matrix;
	}
	
	template < class Component, unsigned Dimensions >
	class Translation_Foo
	{
		private:
			typedef Matrix< Component, Dimensions, 1 > Vector;
			
			static const unsigned Len = Dimensions + 1;
			
			Vector delta;
		
		public:
			Translation_Foo()  {}
			
			Translation_Foo( const Vector& delta ) : delta( delta )  {}
			
			const Vector& Delta() const  { return delta; }
			
			template < class T >
			Matrix< T, 3, 3 >& Initialize( Matrix< T, 3, 3 >& matrix ) const
			{
				IdentityMatrix::Initialize( matrix );
				
				matrix.Cell( 0, 2 ) = delta[ X ];
				matrix.Cell( 1, 2 ) = delta[ Y ];
				
				return matrix;
			}
			
			template < class T >
			Matrix< T, 4, 4 >& Initialize( Matrix< T, 4, 4 >& matrix ) const
			{
				IdentityMatrix::Initialize( matrix );
				
				matrix.Cell( 0, 3 ) = delta[ X ];
				matrix.Cell( 1, 3 ) = delta[ Y ];
				matrix.Cell( 2, 3 ) = delta[ Z ];
				
				return matrix;
			}
	};
	
	typedef Translation_Foo< double, 3 > Translation3D;
	
	
	class Rotation_Base
	{
		private:
			const Radians angle;
			
		protected:
			const double cosine;
			const double sine;
		
		public:
			Rotation_Base( Radians angle ) : angle (           angle   ),
			                                 cosine( std::cos( angle ) ),
			                                 sine  ( std::sin( angle ) )  {}
			
			Radians Angle() const  { return angle; }
	};
	
	template < unsigned Len, unsigned Axis_1, unsigned Axis_2 >
	class Rotation : public Rotation_Base
	{
		public:
			Rotation() : Rotation_Base( Radians() )  {}
			
			Rotation( Radians angle ) : Rotation_Base( angle )  {}
			
			template < class Component >
			Matrix< Component, Len, Len >& Initialize( Matrix< Component, Len, Len >& matrix ) const
			{
				IdentityMatrix::Initialize( matrix );
				
				matrix.Cell( Axis_1, Axis_1 ) =  cosine;
				matrix.Cell( Axis_1, Axis_2 ) = -sine;
				matrix.Cell( Axis_2, Axis_1 ) =  sine;
				matrix.Cell( Axis_2, Axis_2 ) =  cosine;
				
				return matrix;
			}
	};
	
	template < unsigned Len, unsigned Axis_1, unsigned Axis_2 >
	inline Rotation< Len, Axis_1, Axis_2 > operator-( const Rotation< Len, Axis_1, Axis_2 >& rotation )
	{
		return Rotation< Len, Axis_1, Axis_2 >( -rotation.Angle() );
	}
	
	typedef Rotation< 3, 0, 1 > Rotation2D;  // theta
	
	typedef Rotation< 4, 0, 1 > Yaw;    // theta
	typedef Rotation< 4, 1, 2 > Pitch;  // phi
	typedef Rotation< 4, 2, 0 > Roll;   // psi
	
	
	template < class Num >
	class Scale_T
	{
		private:
			Num factor;
		
		public:
			Scale_T() {}
			Scale_T( Num inFactor ) : factor( inFactor )  {}
			
			Scale_T< Num > Inverse() const  { return Scale_T< Num >( 1/factor ); }
			
			Matrix< Num, 4, 4 > Make() const;
	};
	
	typedef Scale_T< double > Scale;
	
	template < class Num >
	Matrix< Num, 4, 4 > Scale_T< Num >::Make() const
	{
		Matrix< Num, 4, 4 > matrix = IdentityMatrix::Make< Num, 4 >();
		
		matrix.Cell( 0, 0 ) = factor;
		matrix.Cell( 1, 1 ) = factor;
		matrix.Cell( 2, 2 ) = factor;
		
		return matrix;
	}
	
}

#endif

