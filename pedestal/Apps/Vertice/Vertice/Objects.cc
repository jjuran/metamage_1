/*	==================
 *	Vertice/Objects.cc
 *	==================
 */

#include "Vertice/Objects.hh"

// Vectoria
#include "Vectoria/Coordinates.hh"
#include "Vectoria/Transform.hh"


namespace Vertice
{
	
	using Vectoria::X;
	using Vectoria::Y;
	using Vectoria::Z;
	
	void Moveable::ContextTranslate( double dx, double dy, double dz )
	{
		V::Translation trans( dx, dy, dz );
		
		itsTransform = Compose( trans.Make(), itsTransform           );
		itsInverse   = Compose( itsInverse,   trans.Inverse().Make() );
	}
	
	void Moveable::ContextTranslate( const V::Vector3D::Type& vec )
	{
		ContextTranslate( vec[ X ], vec[ Y ], vec[ Z ] );
	}
	
	void Moveable::ContextYaw( double dtheta )
	{
		// Thing's orientation is expressed as yaw-pitch-roll
		// from its context's point of view.
		// Delta theta is from the thing's point of view.
		// If you think of yaw-pitch-roll as a series of transforms
		// that convert from context coordinates to thing coordinates,
		// then we're adding another yaw to the end.
		// So we just multiply onto the existing orientation transform.
		
		V::Yaw yaw = V::Radians( dtheta );
		
		itsTransform = Compose( yaw,        itsTransform );
		itsInverse   = Compose( itsInverse, -yaw         );
	}
	
	void Moveable::ContextPitch( double dphi )
	{
		V::Pitch pitch = V::Radians( dphi );
		
		itsTransform = Compose( pitch,      itsTransform  );
		itsInverse   = Compose( itsInverse, -pitch        );
	}
	
	void Moveable::ContextRoll( double dpsi )
	{
		V::Roll roll = V::Radians( dpsi );
		
		itsTransform = Compose( roll,       itsTransform );
		itsInverse   = Compose( itsInverse, -roll        );
	}
	
	void Moveable::ContextScale( double factor )
	{
		V::Scale scale( factor );
		
		itsTransform = Compose( scale.Make(), itsTransform           );
		itsInverse   = Compose( itsInverse,   scale.Inverse().Make() );
	}
	
	void Moveable::LocalTranslate( double dx, double dy, double dz )
	{
		V::Translation trans( dx, dy, dz );
		
		itsTransform = Compose( itsTransform,           trans.Make() );
		itsInverse   = Compose( trans.Inverse().Make(), itsInverse   );
	}
	
	void Moveable::LocalTranslate( const V::Vector3D::Type& vec )
	{
		LocalTranslate( vec[ X ], vec[ Y ], vec[ Z ] );
	}
	
	void Moveable::LocalYaw( double dtheta )
	{
		V::Yaw yaw = V::Radians( dtheta );
		
		itsTransform = Compose( itsTransform, yaw        );
		itsInverse   = Compose( -yaw,         itsInverse );
	}
	
	void Moveable::LocalPitch( double dphi )
	{
		V::Pitch pitch = V::Radians( dphi );
		
		itsTransform = Compose( itsTransform, pitch      );
		itsInverse   = Compose( -pitch,       itsInverse );
	}
	
	void Moveable::LocalRoll( double dpsi )
	{
		V::Roll roll = V::Radians( dpsi );
		
		itsTransform = Compose( itsTransform, roll       );
		itsInverse   = Compose( -roll,        itsInverse );
	}
	
	void Moveable::LocalScale( double factor )
	{
		V::Scale scale( factor );
		
		itsTransform = Compose( itsTransform,           scale.Make() );
		itsInverse   = Compose( scale.Inverse().Make(), itsInverse   );
	}
	
}

