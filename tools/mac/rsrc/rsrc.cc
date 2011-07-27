/*
	rsrc.cc
	-------
*/

// gear
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
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace n = nucleus;
namespace N = Nitrogen;


namespace tool
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace o = orion;
	
	
	static const char*  the_type_opt = NULL;
	static const char*  the_id_opt   = NULL;
	
	static Mac::ResType  the_type;
	static Mac::ResID    the_id;
	
	static HFSUniStr255 the_fork_name;
	
	
	static n::owned< N::ResFileRefNum > open_res_file( const FSRef& ref )
	{
		return N::FSOpenResourceFile( ref,
		                              the_fork_name.length,
		                              the_fork_name.unicode,
		                              Mac::fsRdPerm );
	}
	
	static void list_rsrc_by_handle( N::Handle h )
	{
		N::GetResInfo_Result resInfo = N::GetResInfo( h );
		
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
	
	static void print_rsrc_by_handle( N::Handle h, bool showing )
	{
		if ( !TARGET_RT_MAC_MACHO )
		{
			N::HNoPurge( h );  // not needed on OS X
		}
		
		const char* data = *h.Get();
		
		std::size_t size = N::GetHandleSize( h );
		
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
		
		Mac::Handle h = N::Get1Resource( the_type, the_id );
		
		print_rsrc_by_handle( h, showing );
	}
	
	int Main( int argc, char** argv )
	{
		bool use_data_fork = false;
		
		o::bind_option_to_variable( "--data", use_data_fork );
		
		o::bind_option_to_variable( "-t", the_type_opt );
		
		o::bind_option_to_variable( "--id", the_id_opt );
		
		o::get_options( argc, argv );
		
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
		
		char const *const *free_args = o::free_arguments();
		
		std::size_t n_args = o::free_argument_count();
		
		if ( n_args < 2 )
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

