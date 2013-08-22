// SystemConsole.cc

#include "Genie/SystemConsole.hh"

// POSIX
#include <fcntl.h>

// Iota
#include "iota/strings.hh"

// vfs
#include "vfs/node.hh"
#include "vfs/functions/resolve_pathname.hh"
#include "vfs/primitives/hardlink.hh"
#include "vfs/primitives/remove.hh"
#include "vfs/primitives/symlink.hh"
#include "vfs/primitives/touch.hh"

// Genie
#include "Genie/FS/file-tests.hh"
#include "Genie/FS/open.hh"
#include "Genie/FS/opendir.hh"
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	static ssize_t Spew( const FSTreePtr& file, const char* buffer, std::size_t length )
	{
		return IOHandle_Cast< StreamHandle >( open( file.get(), O_WRONLY | O_TRUNC, 0 ).get() )->Write( buffer, length );
	}
	
	static ssize_t Append( const FSTreePtr& file, const char* buffer, std::size_t length )
	{
		return IOHandle_Cast< StreamHandle >( open( file.get(), O_WRONLY | O_APPEND, 0 ).get() )->Write( buffer, length );
	}
	
	static void MakeWindow( const IOPtr& port_dir )
	{
		FSTreePtr port = port_dir->GetFile();
		
		const vfs::node& cwd = *port;
		
		vfs::node_ptr window = resolve_relative_path( STR_LEN( "window" ), cwd );
		
		if ( exists( window ) )
		{
			return;
		}
		
		vfs::node_ptr view = resolve_relative_path( STR_LEN( "view" ), cwd );
		
		if ( exists( view ) )
		{
			remove( view.get() );
			
			view = resolve_relative_path( STR_LEN( "view" ), cwd );
		}
		
		Spew( resolve_relative_path( STR_LEN( "title" ), cwd ), STR_LEN( "System Console" "\n" ) );
		
		Spew( resolve_relative_path( STR_LEN( "size" ),  cwd ), STR_LEN( "495x272" "\n" ) );
		
		touch( window.get() );
		
		Spew( resolve_relative_path( STR_LEN( "w/text-font" ), cwd ), STR_LEN( "4" "\n" ) );
		Spew( resolve_relative_path( STR_LEN( "w/text-size" ), cwd ), STR_LEN( "9" "\n" ) );
		
		hardlink( *vfs::resolve_absolute_path( STR_LEN( "/gui/new/scrollframe" ) ), *view );
		
		vfs::node_ptr subview = resolve_relative_path( STR_LEN( "v/view" ), cwd );
		
		hardlink( *vfs::resolve_absolute_path( STR_LEN( "/gui/new/frame" ) ), *subview );
		
		vfs::node_ptr subsubview = resolve_relative_path( STR_LEN( "v/v/view" ), cwd );
		
		hardlink( *vfs::resolve_absolute_path( STR_LEN( "/gui/new/textedit" ) ), *subsubview );
		
		symlink( resolve_relative_path( STR_LEN( "v/target" ), cwd ).get(), "v/v" );
		
		Spew( resolve_relative_path( STR_LEN( "v/vertical"  ), cwd ), STR_LEN( "1" "\n" ) );
		Spew( resolve_relative_path( STR_LEN( "v/v/padding" ), cwd ), STR_LEN( "4" "\n" ) );
	}
	
	static FSTreePtr GetConsoleWindow()
	{
		static const vfs::filehandle_ptr the_port = opendir( vfs::resolve_absolute_path( STR_LEN( "/gui/new/port" ) ).get() );
		
		MakeWindow( the_port );
		
		return the_port->GetFile();
	}
	
	static FSTreePtr GetConsoleText()
	{
		vfs::node_ptr text = resolve_relative_path( STR_LEN( "v/v/v/text" ), *GetConsoleWindow() );
		
		return text;
	}
	
	int WriteToSystemConsole( const char* data, unsigned byteCount )
	{
		try
		{
			return Append( GetConsoleText(), data, byteCount );
		}
		catch ( ... )
		{
		}
		
		errno = EIO;
		
		return -1;
	}
	
}

