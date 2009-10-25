/*	===============
 *	DynamicGroup.hh
 *	===============
 */

#ifndef GENIE_IO_DYNAMICGROUP_HH
#define GENIE_IO_DYNAMICGROUP_HH

// Standard C++
#include <map>

// Genie
#include "Genie/IO/Base.hh"


namespace Genie
{
	
	typedef std::size_t DynamicElementID;
	
	typedef std::map< DynamicElementID, boost::weak_ptr< IOHandle > > DynamicGroup;
	
	template < class Handle >
	DynamicGroup& GetDynamicGroup()
	{
		static DynamicGroup gGroup;
		
		return gGroup;
	}
	
	boost::shared_ptr< IOHandle > GetDynamicElementFromGroupByID( const DynamicGroup&  group,
	                                                              DynamicElementID     id );
	
	template < class Handle >
	boost::shared_ptr< IOHandle > GetDynamicElementByID( DynamicElementID id )
	{
		DynamicGroup& group( GetDynamicGroup< Handle >() );
		
		return GetDynamicElementFromGroupByID( group, id );
	}
	
}

#endif

