/*
	FSTreeCache_Impl.cc
	-------------------
*/

#include "Genie/FS/FSTreeCache_Impl.hh"


namespace Genie
{
	
	void FSTreeCache::push_back( const FSNode& node )
	{
		static_cast< FSTreeCache_Impl* >( this )->push_back( node );
	}
	
}

