/*
	Document.cc
	-----------
*/

#include "TestEdit/Document.hh"

// Standard C++
#include <algorithm>

// mac-app-utils
#include "mac_app/Window_menu.hh"

// Nitrogen
#include "Nitrogen/MacWindows.hh"

// MacFiles
#include "MacFiles/Classic.hh"
#include "MacFiles/Unicode.hh"

// Pedestal
#include "Pedestal/WindowStorage.hh"


namespace TestEdit
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	static inline SInt32 ReadAll( N::FSFileRefNum input, char* buffer, SInt32 n )
	{
		return N::FSRead( input, n, buffer, N::ThrowEOF_Always() );
	}
	
	static inline ByteCount ReadAll( N::FSForkRefNum input, char* buffer, ByteCount n )
	{
		return N::FSReadFork( input, n, buffer, N::ThrowEOF_Always() );
	}
	
	template < class FileSpec >
	static plus::string ReadFileData( const FileSpec& file )
	{
		typedef io::filespec_traits< FileSpec > traits;
		
		typedef typename traits::stream     stream;
		typedef typename traits::byte_count byte_count;
		
		plus::string result;
		
		n::owned< stream > input = io::open_for_reading( file );
		
		const std::size_t size = io::get_file_size( input );
		
		char* p = result.reset( size );
		
		const byte_count bytes_read = ReadAll( input, p, size );
		
		// Allow LF newlines
		std::replace( p,
		              p + size,
		              '\n',
		              '\r' );
		
		return result;
	}
	
	static n::owned< CFStringRef > GetFilenameAsCFString( const FSRef& file )
	{
		N::FSGetCatalogInfo_Result info = N::FSGetCatalogInfo( file, kFSCatInfoNone );
		
		return n::convert< n::owned< CFStringRef > >( info.outName );
	}
	
	
	Document::Document()
	: 
		itsWindow( NewWindow() ),
		itHasFile( false ),
		itIsDirty( false )   // A new document is never dirty, even if not saved
	{
	}
	
	static
	void LoadText( WindowRef window, const plus::string& text )
	{
		Ped::View* view = Ped::get_window_view( window );
		
		View& scrollframe = reinterpret_cast< View& >( *view );
		
		SetText( scrollframe, text );
	}
	
	Document::Document( const FSSpec& file )
	: 
		itsWindow( NewWindow( file.name ) ),
		itHasFile( true  ),
		itIsDirty( false )
	{
		LoadText( itsWindow, ReadFileData( file ) );
	}
	
	Document::Document( const FSRef& file )
	: 
		itsWindow( NewWindow() ),
		itHasFile( true  ),
		itIsDirty( false )
	{
		WindowRef window = itsWindow.get();
		
		N::SetWindowTitleWithCFString( window, GetFilenameAsCFString( file ) );
		
		mac::app::Window_menu_remove( window );
		mac::app::Window_menu_insert( window );
		
		LoadText( itsWindow, ReadFileData( file ) );
	}
	
	plus::string Document::GetName() const
	{
		return plus::string( N::GetWTitle( GetWindowRef() ) );
	}
	
}
