/*
	node.cc
	-------
*/

#include "vfs/node.hh"

// plus
#include "plus/hexidecimal.hh"


namespace vfs
{
	
	node::node()
	:
		its_owner     (),
		its_mode      (),
		its_methods   (),
		its_extra     (),
		its_destructor()
	{
	}
	
	static inline plus::string name_of_ptr( const void* ptr )
	{
		return plus::encode_32_bit_hex( (uintptr_t) ptr );
	}
	
	node::node( const node*             owner,
	            const plus::string&     name,
	            mode_t                  mode,
	            const node_method_set*  methods,
	            std::size_t             n_extra,
	            node_destructor         dtor )
	:
		its_owner  ( owner   ),
		its_name   ( name[0] == '/' ? name_of_ptr( this )
		                            : name ),
		its_mode   ( mode    ),
		its_methods( methods ),
		its_extra  ( n_extra ? ::operator new( n_extra ) : NULL ),
		its_destructor( dtor )
	{
		if ( owner )
		{
			intrusive_ptr_add_ref( owner );
		}
	}
	
	node::~node()
	{
		if ( its_destructor )
		{
			its_destructor( this );
		}
		
		::operator delete( its_extra );
		
		if ( its_owner )
		{
			intrusive_ptr_release( its_owner );
		}
	}
	
}

