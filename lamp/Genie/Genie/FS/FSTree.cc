/*	=========
 *	FSTree.cc
 *	=========
 */

#include "Genie/FS/FSTree.hh"

// plus
#include "plus/hexidecimal.hh"
#include "plus/string/concat.hh"

// Genie
#include "Genie/FS/node_method_set.hh"


namespace Genie
{
	
	const FSTreePtr null_FSTreePtr = FSTreePtr();
	
	
	FSTree::FSTree()
	:
		itsParent(),
		itsName(),
		itsMode(),
		its_methods(),
		its_extra(),
		its_destructor()
	{
	}
	
	static inline plus::string NameFromPtr( const FSTree* ptr )
	{
		return plus::encode_32_bit_hex( (unsigned) ptr );
	}
	
	FSTree::FSTree( const FSTree*           parent,
	                const plus::string&     name,
	                mode_t                  mode,
	                const node_method_set*  methods,
	                std::size_t             n_extra,
	                node_destructor         dtor )
	:
		itsParent( parent ),
		itsName  ( name[0] == '/' ? NameFromPtr( this )
		                          : name ),
		itsMode  ( mode ),
		its_methods( methods ),
		its_extra( n_extra ? ::operator new( n_extra ) : NULL ),
		its_destructor( dtor )
	{
	}
	
	FSTree::~FSTree()
	{
		if ( its_destructor )
		{
			its_destructor( this );
		}
		
		::operator delete( its_extra );
	}
	
}

