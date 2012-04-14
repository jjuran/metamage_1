// SystemConsole.cc

#include "Genie/SystemConsole.hh"

// POSIX
#include <fcntl.h>

// Iota
#include "iota/strings.hh"

// vfs
#include "vfs/primitives/hardlink.hh"
#include "vfs/primitives/remove.hh"
#include "vfs/primitives/symlink.hh"

// Genie
#include "Genie/FS/chdir.hh"
#include "Genie/FS/file-tests.hh"
#include "Genie/FS/open.hh"
#include "Genie/FS/touch.hh"
#include "Genie/FS/ResolvePathname.hh"
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
		
		FSTreePtr window = ResolveRelativePath( STR_LEN( "window" ),  port );
		
		if ( exists( window ) )
		{
			return;
		}
		
		FSTreePtr view = ResolveRelativePath( STR_LEN( "view" ), port );
		
		if ( exists( view ) )
		{
			remove( view.get() );
			
			view = ResolveRelativePath( STR_LEN( "view" ), port );
		}
		
		Spew( ResolveRelativePath( STR_LEN( "title" ), port ), STR_LEN( "System Console" "\n" ) );
		
		Spew( ResolveRelativePath( STR_LEN( "size" ),  port ), STR_LEN( "495x272" "\n" ) );
		
		touch( window.get() );
		
		Spew( ResolveRelativePath( STR_LEN( "window/text-font" ), port ), STR_LEN( "4" "\n" ) );
		Spew( ResolveRelativePath( STR_LEN( "window/text-size" ), port ), STR_LEN( "9" "\n" ) );
		
		hardlink( ResolveAbsolutePath( STR_LEN( "/gui/new/scrollframe" ) ).get(), view.get() );
		
		FSTreePtr subview = ResolveRelativePath( STR_LEN( "view/v" ), port );
		
		hardlink( ResolveAbsolutePath( STR_LEN( "/gui/new/frame" ) ).get(), subview.get() );
		
		FSTreePtr subsubview = ResolveRelativePath( STR_LEN( "view/v/v" ), port );
		
		hardlink( ResolveAbsolutePath( STR_LEN( "/gui/new/textedit" ) ).get(), subsubview.get() );
		
		symlink( ResolveRelativePath( STR_LEN( "view/target" ), port ).get(), "v/v" );
		
		Spew( ResolveRelativePath( STR_LEN( "view/vertical"  ), port ), STR_LEN( "1" "\n" ) );
		Spew( ResolveRelativePath( STR_LEN( "view/v/padding" ), port ), STR_LEN( "4" "\n" ) );
	}
	
	static FSTreePtr GetConsoleWindow()
	{
		static IOPtr the_port = chdir( ResolveAbsolutePath( STR_LEN( "/gui/new/port" ) ).get() );
		
		MakeWindow( the_port );
		
		return the_port->GetFile();
	}
	
	static FSTreePtr GetConsoleText()
	{
		FSTreePtr text = ResolveRelativePath( STR_LEN( "view/v/v/text" ), GetConsoleWindow() );
		
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

