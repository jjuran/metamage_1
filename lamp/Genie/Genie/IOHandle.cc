/*	===========
 *	IOHandle.cc
 *	===========
 */

#include "Genie/IOHandle.hh"

// Io
#include "Io/Exceptions.hh"

// Genie
#include "Genie/ResourceTable.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	
	typedef ResourceTable< Io::Handle > IOTable;
	
	void RegisterGenericIORefMod();
	void RegisterGenericIORefMod()
	{
		RegisterIOType( kGenericIODescriptor,
		                IOTable::RefMod,
		                IOTable::Read,
		                IOTable::Write );
	}
	
	
	IORef NewGenericIO( const Io::Handle& io )
	{
		N::OnlyOnce< RegisterGenericIORefMod >();
		
		std::auto_ptr< Io::Handle > newIO( new Io::Handle( io ) );
		
		std::size_t offset = IOTable::Add( newIO );
		
		return IORef( kGenericIODescriptor, offset );
	}
	
}

