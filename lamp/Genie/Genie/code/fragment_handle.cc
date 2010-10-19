/*
	fragment_handle.cc
	------------------
*/

#include "Genie/code/fragment_handle.hh"

// Nitrogen
#include "Nitrogen/CodeFragments.hh"
#include "Nitrogen/MacMemory.hh"


namespace Genie
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	
	
	static n::owned< CFragConnectionID > connect( const Mac::Handle  h )
	{
		const std::size_t size = N::GetHandleSize( h );
		
		return N::GetMemFragment< N::kPrivateCFragCopy >( *h, size );
	}
	
	
	fragment_handle::fragment_handle( const execution_unit& exec )
	:
		loaded_handle( exec ),
		its_fragment_connection( connect( exec.get() ) )
	{
	}
	
	fragment_handle::~fragment_handle()
	{
	}
	
	lamp_entry fragment_handle::get_main_entry_point() const
	{
		const unsigned char* symbol_name = "\p" "_lamp_main";
		
		lamp_entry entry = NULL;
		
		N::FindSymbol( its_fragment_connection, symbol_name, &entry );
		
		return entry;
	}
	
}

