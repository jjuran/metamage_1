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
	
	
	V::XMatrix Camera::WorldToEyeTransform( const Model& model ) const
	{
		V::XMatrix inverse = model.GetSubcontext( fContextIndex ).inverse;
		
		std::size_t index = model.GetSuperContext( fContextIndex );
		
		while ( model.SuperContextExists( index ) )
		{
			inverse = Compose( model.GetSubcontext( index ).inverse,
			                   inverse );
			
			index = model.GetSuperContext( index );
		}
		
		return inverse;
	}
	
	V::XMatrix Camera::EyeToWorldTransform( const Model& model ) const
	{
		V::XMatrix xform = model.GetSubcontext( fContextIndex ).xform;
		
		std::size_t index = model.GetSuperContext( fContextIndex );
		
		while ( model.SuperContextExists( index ) )
		{
			xform = Compose( xform,
			                 model.GetSubcontext( index ).xform );
			
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
	
	Model::Model()
	:
		fContexts( 1 )
	{
		fNameIndex[ " " ] = 0;
	}
	
	Context const& Model::GetContext( std::size_t index ) const
	{
		return fContexts[ index ];
	}
	
	Context& Model::GetContext( std::size_t index )
	{
		return fContexts[ index ];
	}
	
	Moveable const& Model::GetSubcontext( std::size_t index ) const
	{
		return GetContext( index );
	}
	
	Moveable& Model::GetSubcontext( std::size_t index )
	{
		return GetContext( index );
	}
	
	std::size_t Model::AddSubcontext( std::size_t         super,
	                                  const std::string&  name,
	                                  const V::XMatrix&   offset,
	                                  const V::XMatrix&   inv )
	{
		std::size_t result = fContexts.size();  // size == index of next element
		
		fNameIndex[ name ] = result;
		
		fContexts.push_back( Context( super, name, offset, inv ) );
		
		GetContext( super ).AddSubcontext( result );
		
		return result;
	}
	
	std::size_t Model::GetSuperContext( size_t index ) const
	{
		return fContexts[ index ].ParentIndex();
	}
	
	bool Model::SuperContextExists( std::size_t index ) const
	{
		return fContexts[ index ].ParentIndex() < index;  // False for root context
	}
	
}

