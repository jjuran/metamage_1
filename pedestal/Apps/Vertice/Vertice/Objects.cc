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
		
		xform   = Compose( trans.Make(),   xform           );
		inverse = Compose( inverse, trans.Inverse().Make() );
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
		
		xform   = Compose( yaw,     xform );
		inverse = Compose( inverse, -yaw  );
	}
	
	void Moveable::ContextPitch( double dphi )
	{
		V::Pitch pitch = V::Radians( dphi );
		
		xform   = Compose( pitch,   xform  );
		inverse = Compose( inverse, -pitch );
	}
	
	void Moveable::ContextRoll( double dpsi )
	{
		V::Roll roll = V::Radians( dpsi );
		
		xform   = Compose( roll,    xform );
		inverse = Compose( inverse, -roll );
	}
	
	void Moveable::ContextScale( double factor )
	{
		V::Scale scale( factor );
		
		xform   = Compose( scale.Make(),   xform           );
		inverse = Compose( inverse, scale.Inverse().Make() );
	}
	
	void Moveable::LocalTranslate( double dx, double dy, double dz )
	{
		V::Translation trans( dx, dy, dz );
		
		xform   = Compose( xform,           trans.Make()   );
		inverse = Compose( trans.Inverse().Make(), inverse );
	}
	
	void Moveable::LocalTranslate( const V::Vector3D::Type& vec )
	{
		LocalTranslate( vec[ X ], vec[ Y ], vec[ Z ] );
	}
	
	void Moveable::LocalYaw( double dtheta )
	{
		V::Yaw yaw = V::Radians( dtheta );
		
		xform   = Compose( xform, yaw     );
		inverse = Compose( -yaw,  inverse );
	}
	
	void Moveable::LocalPitch( double dphi )
	{
		V::Pitch pitch = V::Radians( dphi );
		
		xform   = Compose( xform,  pitch   );
		inverse = Compose( -pitch, inverse );
	}
	
	void Moveable::LocalRoll( double dpsi )
	{
		V::Roll roll = V::Radians( dpsi );
		
		xform   = Compose( xform, roll    );
		inverse = Compose( -roll, inverse );
	}
	
	void Moveable::LocalScale( double factor )
	{
		V::Scale scale( factor );
		
		xform   = Compose( xform,           scale.Make()   );
		inverse = Compose( scale.Inverse().Make(), inverse );
	}
	
}

