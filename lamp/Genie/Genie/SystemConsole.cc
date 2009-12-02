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
	
	static void MakeWindow( const boost::shared_ptr< IOHandle >& window_dir )
	{
		FSTreePtr window = window_dir->GetFile();
		
		FSTreePtr ref = ResolveRelativePath( STR_LEN( "ref" ),  window );
		
		if ( ref->Exists() )
		{
			return;
		}
		
		FSTreePtr view = ResolveRelativePath( STR_LEN( "view" ), window );
		
		Spew( ResolveRelativePath( STR_LEN( "title" ), window ), STR_LEN( "System Console" "\n" ) );
		
		Spew( ResolveRelativePath( STR_LEN( "size" ),  window ), STR_LEN( "495x272" "\n" ) );
		
		ref->SetTimes();
		
		Spew( ResolveRelativePath( STR_LEN( "text-font" ), ref ), STR_LEN( "4" "\n" ) );
		Spew( ResolveRelativePath( STR_LEN( "text-size" ), ref ), STR_LEN( "9" "\n" ) );
		
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
		static boost::shared_ptr< IOHandle > gWindow = ResolveAbsolutePath( STR_LEN( "/new/window" ) )->ChangeToDirectory();
		
		MakeWindow( gWindow );
		
		return gWindow->GetFile();
	}
	
	static FSTreePtr GetConsoleText()
	{
		FSTreePtr text = ResolveRelativePath( STR_LEN( "view/v/v/text" ), GetConsoleWindow() );
		
		return text;
	}
	
	int WriteToSystemConsole( const char* data, unsigned byteCount )
	{
		return Append( GetConsoleText(), data, byteCount );
	}
	
}

