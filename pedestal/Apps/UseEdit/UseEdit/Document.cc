/*	===========
 *	Document.cc
 *	===========
 */

#include "UseEdit/Document.hh"

// Nucleus
#include "Nucleus/NAssert.h"


namespace UseEdit
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	static std::string ReadFileData( const FSSpec& file )
	{
		using N::fsRdPerm;
		
		NN::Owned< N::FSFileRefNum > fileH( N::FSpOpenDF( file, fsRdPerm ) );
		
		std::size_t fileSize = N::GetEOF( fileH );
		
		std::string data;
		data.resize( fileSize );
		
		int bytes = N::FSRead( fileH, fileSize, &data[ 0 ] );
		
		ASSERT(bytes == fileSize);
		
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
		fFile     ( file    ),
		fHasFile  ( true    ),
		fDirtyFlag( false   )
	{
		std::string contents = ReadFileData( file );
		
		fWindow.Get().SetName( file.name );
		
		fWindow.Get().SubView().ScrollView().AppendChars( contents.data(), contents.size() );
	}
	
}

