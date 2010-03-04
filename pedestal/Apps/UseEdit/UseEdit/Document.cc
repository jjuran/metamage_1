/*	===========
 *	Document.cc
 *	===========
 */

#include "UseEdit/Document.hh"

// Io
#include "io/slurp.hh"

// MacFiles
#include "MacFiles/Classic.hh"
#include "MacFiles/Unicode.hh"


namespace UseEdit
{
	
	namespace n = nucleus;
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace Ped = Pedestal;
	
	template < class FileSpec >
	static std::string ReadFileData( const FileSpec& file )
	{
		std::string data = io::slurp_file< NN::StringFlattener< std::string > >( file );
		
		// Allow LF newlines
		std::replace( data.begin(),
		              data.end(),
		              '\n',
		              '\r' );
		
		return data;
	}
	
	static NN::Owned< N::CFStringRef > GetFilenameAsCFString( const FSRef& file )
	{
		N::FSGetCatalogInfo_Result info = N::FSGetCatalogInfo( file, kFSCatInfoNone );
		
		return n::convert< NN::Owned< N::CFStringRef > >( info.outName );
	}
	
	
	Document::Document()
	: 
		itsWindow( NewWindow() ),
		itHasFile( false ),
		itIsDirty( false )   // A new document is never dirty, even if not saved
	{
	}
	
	static void LoadText( Ped::Window& window, const std::string& text )
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
	
}

