// SystemConsole.cc

#include "Genie/SystemConsole.hh"

// POSIX
#include <fcntl.h>

// Iota
#include "iota/strings.hh"

// Genie
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/IO/Stream.hh"


namespace Genie
{
	
	static ssize_t Spew( const FSTreePtr& file, const char* buffer, std::size_t length )
	{
		return IOHandle_Cast< StreamHandle >( file->Open( O_WRONLY | O_TRUNC ).get() )->Write( buffer, length );
	}
	
	static ssize_t Append( const FSTreePtr& file, const char* buffer, std::size_t length )
	{
		return IOHandle_Cast< StreamHandle >( file->Open( O_WRONLY | O_APPEND ).get() )->Write( buffer, length );
	}
	
	static void MakeWindow( const boost::shared_ptr< IOHandle >& port_dir )
	{
		FSTreePtr port = port_dir->GetFile();
		
		FSTreePtr window = ResolveRelativePath( STR_LEN( "window" ),  port );
		
		if ( window->Exists() )
		{
			return;
		}
		
		FSTreePtr view = ResolveRelativePath( STR_LEN( "view" ), port );
		
		Spew( ResolveRelativePath( STR_LEN( "title" ), port ), STR_LEN( "System Console" "\n" ) );
		
		Spew( ResolveRelativePath( STR_LEN( "size" ),  port ), STR_LEN( "495x272" "\n" ) );
		
		window->SetTimes();
		
		Spew( ResolveRelativePath( STR_LEN( "window/text-font" ), port ), STR_LEN( "4" "\n" ) );
		Spew( ResolveRelativePath( STR_LEN( "window/text-size" ), port ), STR_LEN( "9" "\n" ) );
		
		ResolveAbsolutePath( STR_LEN( "/new/scrollframe" ) )->HardLink( view );
		
		FSTreePtr subview = ResolveRelativePath( STR_LEN( "v" ), view );
		
		ResolveAbsolutePath( STR_LEN( "/new/frame" ) )->HardLink( subview );
		
		FSTreePtr subsubview = ResolveRelativePath( STR_LEN( "v" ), subview );
		
		ResolveAbsolutePath( STR_LEN( "/new/textedit" ) )->HardLink( subsubview );
		
		ResolveRelativePath( STR_LEN( "target" ), view )->SymLink( "v/v" );
		
		Spew( ResolveRelativePath( STR_LEN( "vertical" ), view    ), STR_LEN( "1" "\n" ) );
		Spew( ResolveRelativePath( STR_LEN( "padding"  ), subview ), STR_LEN( "4" "\n" ) );
	}
	
	static FSTreePtr GetConsoleWindow()
	{
		static boost::shared_ptr< IOHandle > the_port = ResolveAbsolutePath( STR_LEN( "/new/port" ) )->ChangeToDirectory();
		
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

