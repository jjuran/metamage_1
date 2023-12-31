/*
	fragment_handle.cc
	------------------
*/

#include "Genie/code/fragment_handle.hh"

// Standard C
#include <stdio.h>

// more-libc
#include "more/string.h"

// mac-glue-utils
#include "mac_glue/Memory.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"



namespace Genie
{
	
	namespace p7 = poseven;
	
	static
	CFragConnectionID connect( Handle h, vfs::relix_entry* main )
	{
		const std::size_t size = mac::glue::GetHandleSize( h );
		
		Str255 errMessage;
		CFragConnectionID connID;
		
		OSErr err = GetMemFragment( *h,
		                            size,
		                            NULL,
		                            kPrivateCFragCopy,
		                            &connID,
		                            (Ptr*) main,
		                            errMessage );
		
		if ( err )
		{
			if ( errMessage[ 0 ] )
			{
				char message[ 256 ];
				
				const Byte* p = errMessage;
				
				char* q = (char*) mempcpy( message, p + 1, p[ 0 ] );
				
				printf( "Code fragment error %d: %s\n", err, message );
			}
			
			Mac::ThrowOSStatus( err );
		}
		
		return connID;
	}
	
	
	fragment_handle::fragment_handle( const execution_unit& exec )
	:
		loaded_handle( exec ),
		its_relix_main(),
		its_fragment_connection( connect( exec.get(), &its_relix_main ) )
	{
		if ( its_relix_main == NULL )
		{
			CloseConnection( &its_fragment_connection );
			
			p7::throw_errno( ENOEXEC );
		}
	}
	
	fragment_handle::~fragment_handle()
	{
		CloseConnection( &its_fragment_connection );
	}
	
	vfs::relix_entry fragment_handle::get_main_entry_point() const
	{
		return its_relix_main;
	}
	
}
