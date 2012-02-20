// SystemConsole.cc

#include "Genie/SystemConsole.hh"

// POSIX
#include <fcntl.h>

// Iota
#include "iota/strings.hh"

// Genie
#include "Genie/FS/file-tests.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	static ssize_t Spew( const FSTreePtr& file, const char* buffer, std::size_t length )
	{
		return IOHandle_Cast< StreamHandle >( file->Open( O_WRONLY | O_TRUNC, 0 ).get() )->Write( buffer, length );
	}
	
	static ssize_t Append( const FSTreePtr& file, const char* buffer, std::size_t length )
	{
		return IOHandle_Cast< StreamHandle >( file->Open( O_WRONLY | O_APPEND, 0 ).get() )->Write( buffer, length );
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
		
		Spew( ResolveRelativePath( STR_LEN( "title" ), port ), STR_LEN( "System Console" "\n" ) );
		
		Spew( ResolveRelativePath( STR_LEN( "size" ),  port ), STR_LEN( "495x272" "\n" ) );
		
		window->SetTimes();
		
		Spew( ResolveRelativePath( STR_LEN( "window/text-font" ), port ), STR_LEN( "4" "\n" ) );
		Spew( ResolveRelativePath( STR_LEN( "window/text-size" ), port ), STR_LEN( "9" "\n" ) );
		
		ResolveAbsolutePath( STR_LEN( "/gui/new/scrollframe" ) )->HardLink( view );
		
		FSTreePtr subview = ResolveRelativePath( STR_LEN( "view/v" ), port );
		
		ResolveAbsolutePath( STR_LEN( "/gui/new/frame" ) )->HardLink( subview );
		
		FSTreePtr subsubview = ResolveRelativePath( STR_LEN( "view/v/v" ), port );
		
		ResolveAbsolutePath( STR_LEN( "/gui/new/textedit" ) )->HardLink( subsubview );
		
		ResolveRelativePath( STR_LEN( "view/target" ), port )->SymLink( "v/v" );
		
		Spew( ResolveRelativePath( STR_LEN( "view/vertical"  ), port ), STR_LEN( "1" "\n" ) );
		Spew( ResolveRelativePath( STR_LEN( "view/v/padding" ), port ), STR_LEN( "4" "\n" ) );
	}
	
	static FSTreePtr GetConsoleWindow()
	{
		static IOPtr the_port = ResolveAbsolutePath( STR_LEN( "/gui/new/port" ) )->ChangeToDirectory();
		
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

