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
	
	static std::string ReadFileData( const FSSpec& file );
	static std::string ReadFileData( const FSRef & file );
	
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
		
		return data;
	}
	
	
	Document::Document( Ped::WindowClosure& closure )
	: 
		fWindow   ( closure ),
		fHasFile  ( false   ),
		fDirtyFlag( false   )   // A new document is never dirty, even if not saved
	{
	}
	
	Document::Document( Ped::WindowClosure& closure, const FSSpec& file )
	: 
		fWindow   ( closure ),
		fHasFile  ( true    ),
		fDirtyFlag( false   )
	{
		std::string contents = ReadFileData( file );
		
		fWindow.Get().SetName( file.name );
		
		fWindow.Get().SubView().ScrollView().AppendChars( contents.data(), contents.size() );
	}
	
	Document::Document( Ped::WindowClosure& closure, const FSRef& file )
	: 
		fWindow   ( closure ),
		fHasFile  ( true    ),
		fDirtyFlag( false   )
	{
		std::string contents = ReadFileData( file );
		
		N::FSGetCatalogInfo_Result info = N::FSGetCatalogInfo( file, kFSCatInfoNone );
		
		N::UniString unistring = NN::Convert< N::UniString >( info.outName );
		
		std::string string( unistring.begin(), unistring.end() );
		
		N::Str255 name( string );
		
		fWindow.Get().SetName( name );
		
		fWindow.Get().SubView().ScrollView().AppendChars( contents.data(), contents.size() );
	}
	
}

