/*
	fragment_handle.cc
	------------------
*/

#include "Genie/code/fragment_handle.hh"

// Standard C
#include <string.h>

// more-libc
#include "more/string.h"

// mac-glue-utils
#include "mac_glue/Memory.hh"

// gear
#include "gear/inscribe_decimal.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Mac/Toolbox/Utilities/ThrowOSStatus.hh"

// relix-kernel
#include "relix/fs/console.hh"


#define STR_LEN( s )  "" s, (sizeof s - 1)


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
				enum
				{
					buffer_size = sizeof "Code fragment error -12345: \n" + 255,
				};
				
				const Byte* p = errMessage;
				
				char message[ buffer_size ];
				
				char* q = message;
				
				q = (char*) mempcpy( q, STR_LEN( "Code fragment error " ) );
				
				const char* errnum = gear::inscribe_decimal( err );
				
				q = (char*) mempcpy( q, errnum, strlen( errnum ) );
				
				q = (char*) mempcpy( q, STR_LEN( ": " ) );
				
				q = (char*) mempcpy( q, p + 1, p[ 0 ] );
				
				q = (char*) mempcpy( q, STR_LEN( "\n" ) );
				
				relix::console::log( message, q - message );
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
