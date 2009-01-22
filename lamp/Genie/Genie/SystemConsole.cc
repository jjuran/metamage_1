// SystemConsole.cc

#include "Genie/SystemConsole.hh"

// POSIX
#include <fcntl.h>

// Iota
#include "iota/strings.hh"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
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
	
	static void MakeWindow( const boost::shared_ptr< IOHandle >& window_dir )
	{
		FSTreePtr window = window_dir->GetFile();
		
		FSTreePtr ref = ResolvePathname( "ref",  window );
		
		if ( ref->Exists() )
		{
			return;
		}
		
		FSTreePtr view = ResolvePathname( "view", window );
		
		Spew( ResolvePathname( "title", window ), STR_LEN( "System Console" "\n" ) );
		
		Spew( ResolvePathname( "size",  window ), STR_LEN( "495x272" "\n" ) );
		
		ref->SetTimes();
		
		Spew( ResolvePathname( "text-font", ref ), STR_LEN( "4" "\n" ) );
		Spew( ResolvePathname( "text-size", ref ), STR_LEN( "9" "\n" ) );
		
		ResolvePathname( "/new/scrollframe" )->HardLink( view );
		
		FSTreePtr subview = ResolvePathname( "v", view );
		
		ResolvePathname( "/new/frame" )->HardLink( subview );
		
		FSTreePtr subsubview = ResolvePathname( "v", subview );
		
		ResolvePathname( "/new/textedit" )->HardLink( subsubview );
		
		ResolvePathname( "target", view )->SymLink( "v/v" );
		
		Spew( ResolvePathname( "vertical", view    ), STR_LEN( "1" "\n" ) );
		Spew( ResolvePathname( "margin",   subview ), STR_LEN( "4" "\n" ) );
	}
	
	static FSTreePtr GetConsoleWindow()
	{
		static boost::shared_ptr< IOHandle > gWindow = ResolvePathname( "/new/window" )->ChangeToDirectory();
		
		MakeWindow( gWindow );
		
		return gWindow->GetFile();
	}
	
	static FSTreePtr GetConsoleText()
	{
		FSTreePtr text = ResolvePathname( "view/v/v/text", GetConsoleWindow() );
		
		return text;
	}
	
	int WriteToSystemConsole( const char* data, std::size_t byteCount )
	{
		return Append( GetConsoleText(), data, byteCount );
	}
	
}

