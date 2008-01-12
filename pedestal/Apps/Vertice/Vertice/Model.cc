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
	
	template < class Key, class Value >
	const Value& Get( const std::map< Key, Value >& aMap, const Key& key )
	{
		typename std::map< Key, Value >::const_iterator it = aMap.find( key );
		
		if ( it == aMap.end() )
		{
			static Value val = Value();
			
			return val;  // FIXME:  Probably should throw or assert
		}
		
		return it->second;
	}
	
	template < class Key, class Value >
	class MapWrapper
	{
		private:
			const std::map< Key, Value >& myMap;
		
		public:
			MapWrapper(const std::map< Key, Value >& aMap ) : myMap( aMap )  {}
			
			const Value& operator[]( const Key& key ) const  { return ( *myMap.find( key ) ).second; }
	};
	
	template < class Key, class Value >
	MapWrapper< Key, Value > MAP( const std::map< Key, Value >& aMap )
	{
		return MapWrapper< Key, Value >( aMap );
	}
	
	
	V::XMatrix Camera::WorldToEyeTransform( const Scene& model ) const
	{
		V::XMatrix inverse = model.GetSubcontext( itsContextIndex ).itsInverse;
		
		std::size_t index = model.GetSuperContext( itsContextIndex );
		
		while ( model.SuperContextExists( index ) )
		{
			inverse = Compose( model.GetSubcontext( index ).itsInverse,
			                   inverse );
			
			index = model.GetSuperContext( index );
		}
		
		return inverse;
	}
	
	V::XMatrix Camera::EyeToWorldTransform( const Scene& model ) const
	{
		V::XMatrix xform = model.GetSubcontext( itsContextIndex ).itsTransform;
		
		std::size_t index = model.GetSuperContext( itsContextIndex );
		
		while ( model.SuperContextExists( index ) )
		{
			xform = Compose( xform,
			                 model.GetSubcontext( index ).itsTransform );
			
			index = model.GetSuperContext( index );
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
	
	Scene::Scene()
	:
		itsContexts( 1 )
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

