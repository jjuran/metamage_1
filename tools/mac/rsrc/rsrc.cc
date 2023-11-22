/*
	rsrc.cc
	-------
*/

// Standard C
#include <errno.h>
#include <stdlib.h>

// command
#include "command/get_option.hh"

// gear
#include "gear/hexadecimal.hh"
#include "gear/parse_decimal.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/var_string.hh"

// MacScribe
#include "quad/utf8_quad_name.hh"
#include "rsrc/utf8_resinfo_name.hh"

// poseven
#include "poseven/functions/perror.hh"
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
		OSStatus err;
		
		err = CoreEndianInstallFlipper( kCoreEndianResourceManagerDomain,
		                                type,
		                                &null_flip_proc,
		                                NULL );
		
		Mac::ThrowOSStatus( err );
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
	
	static n::owned< N::ResFileRefNum > open_res_file( const FSRef& ref )
	{
		return N::FSOpenResourceFile( ref,
		                              the_fork_name.length,
		                              the_fork_name.unicode,
		                              Mac::fsRdPerm );
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
		
		plus::var_string line = name.move();
		
		line += '\n';
		
		p7::write( p7::stdout_fileno, line );
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
			p7::perror( "rsrc", showing ? "show" : "cat", "type and id must be specified" );
			
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
			p7::perror( "rsrc: a command and argument are required", 0 );
			
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
				p7::perror( "rsrc", path, EISDIR );
				
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
			
			p7::perror( "rsrc", path, ENOENT );
			
			return 1;
		}
		
		n::owned< N::ResFileRefNum > resFile;
		
		try
		{
			resFile = open_res_file( ref );
			
		#ifdef __LITTLE_ENDIAN__
			
			install_null_flippers();
			
		#endif
		}
		catch ( const Mac::OSStatus& err )
		{
			switch ( err )
			{
				case eofErr:
					p7::perror( "rsrc", path, "not a resource file" );
					break;
				
				case afpAccessDenied:
					p7::perror( "rsrc", path, EACCES );
					break;
				
				default:
					throw;
			}
			
			return 1;
		}
		
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
		
		p7::perror( "rsrc", cmd, "not a valid command" );
		
		return 1;
	}
	
}
