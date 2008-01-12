/*	================
 *	Vertice/Model.cc
 *	================
 */

#include "Vertice/Model.hh"

// Standard C++
#include <algorithm>

// Nucleus
#include "Nucleus/NAssert.h"

// Vertice
#include "Vectoria/Units.hh"


namespace Vertice
{
	
	V::XMatrix Camera::WorldToEyeTransform( const Scene& scene ) const
	{
		V::XMatrix inverse = scene.GetSubcontext( itsContextIndex ).itsInverse;
		
		std::size_t index = scene.GetSuperContext( itsContextIndex );
		
		while ( scene.SuperContextExists( index ) )
		{
			inverse = Compose( scene.GetSubcontext( index ).itsInverse,
			                   inverse );
			
			index = scene.GetSuperContext( index );
		}
		
		return inverse;
	}
	
	V::XMatrix Camera::EyeToWorldTransform( const Scene& scene ) const
	{
		V::XMatrix xform = scene.GetSubcontext( itsContextIndex ).itsTransform;
		
		std::size_t index = scene.GetSuperContext( itsContextIndex );
		
		while ( scene.SuperContextExists( index ) )
		{
			xform = Compose( xform,
			                 scene.GetSubcontext( index ).itsTransform );
			
			index = scene.GetSuperContext( index );
		}
		
		return xform;
	}
	
	V::XMatrix Camera::EyeToPortTransform() const
	{
		return V::XMatrix( V::Pitch( -V::Degrees( 90 ) ) );
	}
	
	V::XMatrix Camera::PortToEyeTransform() const
	{
		return V::XMatrix( V::Pitch( V::Degrees( 90 ) ) );
	}
	
	Scene::Scene() : itsContexts( 1 )
	{
		itsNameIndex[ " " ] = 0;
	}
	
	Context const& Scene::GetContext( std::size_t index ) const
	{
		return itsContexts[ index ];
	}
	
	Context& Scene::GetContext( std::size_t index )
	{
		return itsContexts[ index ];
	}
	
	Moveable const& Scene::GetSubcontext( std::size_t index ) const
	{
		return GetContext( index );
	}
	
	Moveable& Scene::GetSubcontext( std::size_t index )
	{
		return GetContext( index );
	}
	
	std::size_t Scene::AddSubcontext( std::size_t         super,
	                                  const std::string&  name,
	                                  const V::XMatrix&   offset,
	                                  const V::XMatrix&   inv )
	{
		std::size_t result = itsContexts.size();  // size == index of next element
		
		itsNameIndex[ name ] = result;
		
		itsContexts.push_back( Context( super, name, offset, inv ) );
		
		GetContext( super ).AddSubcontext( result );
		
		return result;
	}
	
	std::size_t Scene::GetSuperContext( size_t index ) const
	{
		return itsContexts[ index ].ParentIndex();
	}
	
	bool Scene::SuperContextExists( std::size_t index ) const
	{
		return itsContexts[ index ].ParentIndex() < index;  // False for root context
	}
	
}

