/*	===========
 *	Document.cc
 *	===========
 */

#include "UseEdit/Document.hh"

// Standard C++
#include <algorithm>

// iota
#include "iota/convert_string.hh"

// Nitrogen
#include "Nitrogen/MacWindows.hh"

// MacFiles
#include "MacFiles/Classic.hh"
#include "MacFiles/Unicode.hh"


namespace UseEdit
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
	
	static void LoadText( Ped::Window& window, const plus::string& text )
	{
		View& scrollframe = reinterpret_cast< View& >( *window.GetView() );
		
		SetText( scrollframe, text );
	}
	
	Document::Document( const FSSpec& file )
	: 
		itsWindow( NewWindow( file.name ) ),
		itHasFile( true  ),
		itIsDirty( false )
	{
		LoadText( *itsWindow, ReadFileData( file ) );
	}
	
	Document::Document( const FSRef& file )
	: 
		itsWindow( NewWindow() ),
		itHasFile( true  ),
		itIsDirty( false )
	{
		N::SetWindowTitleWithCFString( N::FrontWindow(),
		                               GetFilenameAsCFString( file ) );
		
		LoadText( *itsWindow, ReadFileData( file ) );
	}
	
	plus::string Document::GetName() const
	{
		return iota::convert_string< plus::string >( N::GetWTitle( GetWindowRef() ) );
	}
	
}

