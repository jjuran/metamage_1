/*
	worldview/Model.cc
	------------------
*/

#include "worldview/Model.hh"

// Vectoria
#include "Vectoria/Units.hh"


namespace worldview
{
	
	V::XMatrix WorldToEyeTransform( const Scene& scene, std::size_t camera )
	{
		V::XMatrix inverse = scene.GetContext( camera ).itsInverse;
		
		std::size_t index = scene.GetSuperContext( camera );
		
		while ( scene.SuperContextExists( index ) )
		{
			inverse = Compose( scene.GetContext( index ).itsInverse,
			                   inverse );
			
			index = scene.GetSuperContext( index );
		}
		
		return inverse;
	}
	
	V::XMatrix EyeToWorldTransform( const Scene& scene, std::size_t camera )
	{
		V::XMatrix xform = scene.GetContext( camera ).itsTransform;
		
		std::size_t index = scene.GetSuperContext( camera );
		
		while ( scene.SuperContextExists( index ) )
		{
			xform = Compose( xform,
			                 scene.GetContext( index ).itsTransform );
			
			index = scene.GetSuperContext( index );
		}
		
		return xform;
	}
	
	V::XMatrix EyeToPortTransform()
	{
		return V::XMatrix( V::Pitch( -V::Degrees( 90 ) ) );
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
