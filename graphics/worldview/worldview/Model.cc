/*
	worldview/Model.cc
	------------------
*/

#include "worldview/Model.hh"

// Vectoria
#include "Vectoria/Units.hh"


namespace worldview
{
	
	V::XMatrix Camera::WorldToEyeTransform( const Scene& scene ) const
	{
		V::XMatrix inverse = scene.GetContext( itsContextIndex ).itsInverse;
		
		std::size_t index = scene.GetSuperContext( itsContextIndex );
		
		while ( scene.SuperContextExists( index ) )
		{
			inverse = Compose( scene.GetContext( index ).itsInverse,
			                   inverse );
			
			index = scene.GetSuperContext( index );
		}
		
		return inverse;
	}
	
	V::XMatrix Camera::EyeToWorldTransform( const Scene& scene ) const
	{
		V::XMatrix xform = scene.GetContext( itsContextIndex ).itsTransform;
		
		std::size_t index = scene.GetSuperContext( itsContextIndex );
		
		while ( scene.SuperContextExists( index ) )
		{
			xform = Compose( xform,
			                 scene.GetContext( index ).itsTransform );
			
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
	}
	
	Context const& Scene::GetContext( std::size_t index ) const
	{
		return itsContexts[ index ];
	}
	
	Context& Scene::GetContext( std::size_t index )
	{
		return itsContexts[ index ];
	}
	
	std::size_t Scene::AddSubcontext( std::size_t          super,
	                                  const plus::string&  name,
	                                  const V::XMatrix&    offset,
	                                  const V::XMatrix&    inv )
	{
		std::size_t result = itsContexts.size();  // size == index of next element
		
		itsContexts.push_back( Context( super, name, offset, inv ) );
		
		GetContext( super ).AddSubcontext( result );
		
		return result;
	}
	
}
