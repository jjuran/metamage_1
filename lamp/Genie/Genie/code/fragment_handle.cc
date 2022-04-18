/*
	fragment_handle.cc
	------------------
*/

#include "Genie/code/fragment_handle.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/CodeFragments.hh"
#include "Nitrogen/MacMemory.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static n::owned< CFragConnectionID > connect( const Mac::Handle  h,
	                                              vfs::relix_entry*  main )
	{
		const std::size_t size = GetHandleSize( h );
		
		return N::GetMemFragment< N::kPrivateCFragCopy >( *h,
		                                                  size,
		                                                  NULL,
		                                                  main );
	}
	
	
	fragment_handle::fragment_handle( const execution_unit& exec )
	:
		loaded_handle( exec ),
		its_relix_main(),
		its_fragment_connection( connect( exec.get(), &its_relix_main ) )
	{
		if ( its_relix_main == NULL )
		{
			p7::throw_errno( ENOEXEC );
		}
	}
	
	fragment_handle::~fragment_handle()
	{
	}
	
	vfs::relix_entry fragment_handle::get_main_entry_point() const
	{
		return its_relix_main;
	}
	
}
