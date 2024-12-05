/*
	rsrc.cc
	-------
*/

// Standard C
#include <errno.h>
#include <stdlib.h>

// Standard C++
#include <algorithm>

// more-posix
#include "more/perror.hh"

// command
#include "command/get_option.hh"

// mac-rsrc-utils
#include "mac_rsrc/open_res_file.hh"

// gear
#include "gear/hexadecimal.hh"
#include "gear/parse_decimal.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/string.hh"

// MacScribe
#include "quad/utf8_quad_name.hh"
#include "rsrc/utf8_resinfo_name.hh"

// poseven
#include "poseven/functions/write.hh"

// Nitrogen
#include "Nitrogen/Files.hh"
#include "Nitrogen/Resources.hh"

// Orion
#include "Orion/Main.hh"


namespace n = nucleus;
namespace N = Nitrogen;

using namespace command::constants;

enum
{
	Option_type = 't',
	Option_hex  = 'x',
	
	Option_last_byte = 255,
	
	Option_data_fork,
	Option_id,
};

static command::option options[] =
{
	{ "data",      Option_data_fork },
	{ "data-fork", Option_data_fork },
	{ "hex-types", Option_hex       },
	
	{ "id",  Option_id, Param_required },
	{ "",  Option_type, Param_required },
	
	{ NULL }
};

static bool use_data_fork;
static bool use_hex_types;

static const char*  the_type_opt = NULL;
static const char*  the_id_opt   = NULL;

static char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_data_fork:
				use_data_fork = true;
				break;
			
			case Option_hex:
				use_hex_types = true;
				break;
			
			case Option_type:
				the_type_opt = command::global_result.param;
				break;
			
			case Option_id:
				the_id_opt = command::global_result.param;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}


namespace tool
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static Mac::ResType  the_type;
	static Mac::ResID    the_id;
	
	static HFSUniStr255 the_fork_name;
	
	
	/*
		Mac OS X helpfully flips to little-endian the resources we're trying
		to copy and use portably.  Redefine all the relevant resource flippers
		as no-ops so we get the original pristine data.
	*/
	
	static
	OSStatus null_flip_proc( OSType, OSType, SInt16, void*, ByteCount, Boolean, void* )
	{
		return noErr;
	}
	
	static
	void install_null_flipper( ResType type )
	{
	#ifdef __LITTLE_ENDIAN__
		
		OSStatus err;
		
		err = CoreEndianInstallFlipper( kCoreEndianResourceManagerDomain,
		                                type,
		                                &null_flip_proc,
		                                NULL );
		
		Mac::ThrowOSStatus( err );
		
	#endif
	}
	
	static
	void install_null_flippers()
	{
		int n_types = N::Count1Types();
		
		for ( int i = 1;  i <= n_types;  ++i )
		{
			Mac::ResType type = N::Get1IndType( i );
			
			install_null_flipper( type );
		}
	}
	
	static void list_rsrc_by_handle( N::Handle h )
	{
		mac::types::ResInfo resInfo = N::GetResInfo( h );
		
		if ( use_hex_types )
		{
			char buffer[ 4 + 1 + 8 + 1 ];
			
			char* p = buffer;
			
			gear::encode_16_bit_hex( resInfo.id, p );
			
			p += 4;
			
			*p++ = '.';
			
			gear::encode_32_bit_hex( resInfo.type, p );
			
			p += 8;
			
			*p++ = '\n';
			
			p7::write( p7::stdout_fileno, buffer, sizeof buffer );
			
			return;
		}
		
		plus::string name = MacScribe::get_utf8_name_from_ResInfo( resInfo );
		
		const char* data = name.data();
		size_t      size = name.size();
		
		/*
			We created a string from scratch, so
			we're not sharing an extent buffer.
			
			There's always at least one byte beyond
			the end of the string data (for a trailing NUL).
			
			In the event of a maximum-sized small string, we're
			overwriting the byte that marks it as a small string,
			but since that byte remains non-negative (0 -> 10),
			it has no effect on object destruction.
			
			So in all cases, this is safe.
		*/
		
		const_cast< char* >( data )[ size++ ] = '\n';
		
		p7::write( p7::stdout_fileno, data, size );
	}
	
	static void list_rsrc_by_id( Mac::ResType type, Mac::ResID id )
	{
		Mac::Handle h = N::Get1Resource( type, id );
		
		list_rsrc_by_handle( h );
		
		N::ReleaseResource( h );
	}
	
	static void list_rsrcs_of_type( Mac::ResType type )
	{
		if ( the_id_opt )
		{
			list_rsrc_by_id( type, the_id );
		}
		else
		{
			int n_rsrcs = N::Count1Resources( type );
			
			for ( int j = 1;  j <= n_rsrcs;  ++j )
			{
				Mac::Handle h = N::Get1IndResource( type, j );
				
				list_rsrc_by_handle( h );
				
				N::ReleaseResource( h );
			}
		}
	}
	
	static int list_rsrcs()
	{
		if ( the_type_opt )
		{
			list_rsrcs_of_type( the_type );
		}
		else
		{
			int n_types = N::Count1Types();
			
			for ( int i = 1;  i <= n_types;  ++i )
			{
				Mac::ResType type = N::Get1IndType( i );
				
				list_rsrcs_of_type( type );
			}
		}
		
		return 0;
	}
	
	static
	void print_rsrc_by_handle( Handle h, bool showing )
	{
	#if ! TARGET_API_MAC_OSX
		
		HNoPurge( h );  // not available in 64-bit, nor needed on OS X at all
		
	#endif
		
		const char* data = *h;
		
		std::size_t size = GetHandleSize( h );
		
		plus::string printout;
		
		if ( showing )
		{
			char* p = printout.reset( size + 1 );
			
			std::replace_copy( data, data + size, p, '\r', '\n' );
			
			p[ size ] = '\n';
			
			printout = plus::utf8_from_mac( printout );
			
			data = printout.data();
			
			size = printout.size();
		}
		
		p7::write( p7::stdout_fileno, data, size );
	}
	
	static int print_rsrc( bool showing )
	{
		if ( the_type_opt == NULL  ||  the_id_opt == NULL )
		{
			more::perror( "rsrc", showing ? "show" : "cat", "type and id must be specified" );
			
			return 1;
		}
		
		Handle h = N::Get1Resource( the_type, the_id );
		
		print_rsrc_by_handle( h, showing );
		
		return 0;
	}
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		const int argn = argc - (args - argv);
		
		if ( the_type_opt != NULL )
		{
			the_type = Mac::ResType( MacScribe::parse_utf8_quad_name( the_type_opt ) );
		}
		
		if ( the_id_opt != NULL )
		{
			the_id = Mac::ResID( gear::parse_decimal( the_id_opt ) );
		}
		
		if ( !use_data_fork )
		{
			N::ThrowOSStatus( ::FSGetResourceForkName( &the_fork_name ) );
		}
		
		char const *const *free_args = args;
		
		if ( argn < 2 )
		{
			more::perror( "rsrc: a command and argument are required", 0 );
			
			return 1;
		}
		
		const plus::string cmd = free_args[ 0 ];
		
		const char* path = free_args[ 1 ];
		
		FSRef ref;
		
		try
		{
			const N::FSPathMakeRef_Result madeRef = N::FSPathMakeRef( path );
			
			if ( madeRef.isDirectory )
			{
				more::perror( "rsrc", path, EISDIR );
				
				return 1;
			}
			
			ref = madeRef.ref;
		}
		catch ( const Mac::OSStatus& err )
		{
			if ( err != fnfErr )
			{
				throw;
			}
			
			more::perror( "rsrc", path, ENOENT );
			
			return 1;
		}
		
		using mac::rsrc::open_res_file;
		
		n::owned< N::ResFileRefNum > resFile;
		
		ResFileRefNum refnum = open_res_file( ref, the_fork_name, fsRdPerm );
		
		if ( refnum < 0 )
		{
			OSErr err = refnum;
			
			switch ( err )
			{
				case eofErr:
					more::perror( "rsrc", path, "not a resource file" );
					break;
				
				case afpAccessDenied:
					more::perror( "rsrc", path, EACCES );
					break;
				
				default:
					more::perror( "rsrc", path, "unexpected error" );
					break;
			}
			
			return 1;
		}
		
		resFile = n::owned< N::ResFileRefNum >::seize( N::ResFileRefNum( refnum ) );
		
	#ifdef __LITTLE_ENDIAN__
		
		install_null_flippers();
		
	#endif
		
		if ( cmd == "list" )
		{
			return list_rsrcs();
		}
		else if ( cmd == "cat" )
		{
			return print_rsrc( false );
		}
		else if ( cmd == "show" )
		{
			return print_rsrc( true );
		}
		
		more::perror( "rsrc", cmd.c_str(), "not a valid command" );
		
		return 1;
	}
	
}
