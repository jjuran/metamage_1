/*	===========
 *	Document.cc
 *	===========
 */

#include "UseEdit/Document.hh"

// Nucleus
#include "Nucleus/NAssert.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"


namespace UseEdit
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	namespace FS = FileSystem;
	
#if TARGET_API_MAC_CARBON
	
	static std::string ReadFileData( const FSSpec& file )  { return ""; }
	
#else
	
	static std::string ReadFileData( const FSRef & file )  { return ""; }
	
#endif
	
	static std::string ReadFileData( const FS::Spec& file )
	{
		using N::fsRdPerm;
		
		NN::Owned< FS::Stream > fileH( FS::Open( file, fsRdPerm ) );
		
		SInt64 fileSize = FS::GetForkSize( fileH );
		
		// check for negative file size
		
		std::string data;
		data.resize( fileSize );
		
		std::size_t bytes = FS::Read( fileH, &data[ 0 ], fileSize );
		
		ASSERT( bytes == fileSize );
		
		// Allow LF newlines
		std::replace( data.begin(),
		              data.end(),
		              '\n',
		              '\r' );
		
		return data;
	}
	
	
	Document::Document( Ped::WindowClosure& closure )
	: 
		itsWindow( closure ),
		itHasFile( false   ),
		itIsDirty( false   )   // A new document is never dirty, even if not saved
	{
	}
	
	static void LoadText( Window::Base::SubViewType& scroller, const std::string& text )
	{
		Ped::TEView& editor( scroller.ScrolledView() );
		
		editor.AppendChars( text.data(), text.size(), false );
		
		editor.SetSelection( 0, 0 );
		
		scroller.Calibrate();
	}
	
	Document::Document( Ped::WindowClosure& closure, const FSSpec& file )
	: 
		itsWindow( closure ),
		itHasFile( true    ),
		itIsDirty( false   )
	{
		std::string contents = ReadFileData( file );
		
		itsWindow.Get().SetName( file.name );
		
		LoadText( itsWindow.Get().SubView(), contents );
	}
	
	Document::Document( Ped::WindowClosure& closure, const FSRef& file )
	: 
		itsWindow( closure ),
		itHasFile( true    ),
		itIsDirty( false   )
	{
		std::string contents = ReadFileData( file );
		
		N::FSGetCatalogInfo_Result info = N::FSGetCatalogInfo( file, kFSCatInfoNone );
		
		N::UniString unistring = NN::Convert< N::UniString >( info.outName );
		
		std::string string( unistring.begin(), unistring.end() );
		
		N::Str255 name( string );
		
		itsWindow.Get().SetName( name );
		
		LoadText( itsWindow.Get().SubView(), contents );
	}
	
}

