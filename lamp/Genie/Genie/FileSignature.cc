/*	======================
 *	Genie/FileSignature.cc
 *	======================
 */

#include "Genie/FileSignature.hh"

// Standard C++
#include <map>

// Nucleus
#include "Nucleus/Convert.h"

// Nitrogen
#include "Nitrogen/MacTypes.h"

// Genie
#include "Genie/Process.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	::OSType TextFileCreator()
	{
		if ( const char* macEditorSignature = CurrentProcess().GetEnv( "MAC_EDITOR_SIGNATURE" ) )
		{
			try
			{
				return NN::Convert< N::OSType, std::string >( macEditorSignature );
			}
			catch ( ... )
			{
				// Probably not four characters long
			}
		}
		
		const ::OSType sigSimpleText = 'ttxt';
		
		return sigSimpleText;
	}
	
	
	struct ExtensionToTypeRecord
	{
		const char* extension;
		::OSType type;
		const char* comment;
	};
	
	const ExtensionToTypeRecord gExtensionToTypeMappingInput[] =
	{
		// Notes
		// -----
		// .sit is ambiguous: 'SIT!', 'SITD', and 'SIT5' are all possible translations.
		// My copy of Internet Config uses 'SITD'.
		
		// .bin is ambiguous, used for MacBinary and disk images.
		// We'll need to have a generic binary type and a means of autodetection.
		// UPDATE:  To avoid pain, we're going with .mbin / 'mBIN' for new archives.
		
		// Archive formats
		{ "mbin", 'mBIN', "MacBinary III+" " archive" },
		{ "mbi",  'mBIN', "MacBinary III+" " archive" },
		{ "zip",  'ZIP ', "zip"            " archive" },
		{ "tar",  'TARF', "TAR"            " archive" },
		{ "lha",  'LHA ', "LHA"            " archive" },
		{ "rar",  'RARf', "RAR"            " archive" },
		{ "sit",  'SITD', "StuffIt"        " archive" },
		
		// Single-file encoding formats
		{ "as",  'ApSi', "AppleSingle"  "-encoded file" },
		{ "bin", 'BINA', "MacBinary"    "-encoded file" },
		
		// Single-file compression formats
		{ "Z",   'ZIVU', "Z"     "-compressed file" },
		{ "gz",  'Gzip', "gzip"  "-compressed file" },
		{ "bz2", 'Bzp2', "bzip2" "-compressed file" },
		
		// Compressed archive coalesced extensions
		{ "taz", 'ZIVU', "Z"    "-compressed " "tarball" },
		{ "tgz", 'Gzip', "gzip" "-compressed " "tarball" },
		{ "mbz", 'Gzip', "gzip" "-compressed " "MacBinary archive" },
		
		// Image formats
		{ "gif",  'GIFf', "GIF"  " image" },
		{ "png",  'PNGf', "PNG"  " image" },
		{ "bmp",  'BMPf', "BMP"  " image" },
		{ "jpg",  'JPEG', "JPEG" " image" },
		{ "tiff", 'TIFF', "TIFF" " image" },
		{ "pict", 'PICT', "PICT" " image" },
	};
	
	
	typedef std::map< std::string, ::OSType > ExtensionToTypeMapping;
	
	
	static ExtensionToTypeMapping BuildExtensionToTypeMapping()
	{
		ExtensionToTypeMapping result;
		
		const std::size_t extensionCount = sizeof gExtensionToTypeMappingInput / sizeof (ExtensionToTypeRecord);
		
		// Would be nice to use std::for_each() but it's just too much of a PITA.
		for ( const ExtensionToTypeRecord* p = gExtensionToTypeMappingInput;  p < gExtensionToTypeMappingInput + extensionCount;  ++p )
		{
			// ASSERT( p->extension != NULL );
			result[ std::string( p->extension ) ] = p->type;
		}
		
		return result;
	}
	
	static const ExtensionToTypeMapping& GetExtensionToTypeMapping()
	{
		static ExtensionToTypeMapping mapping( BuildExtensionToTypeMapping() );
		
		return mapping;
	}
	
	FileSignature PickFileSignatureForName( const std::string& name )
	{
		std::size_t dot = name.find_last_of( "." );
		
		if ( dot != name.npos )
		{
			std::string extension = name.substr( dot + 1, name.npos );
			
			const ExtensionToTypeMapping& mapping( GetExtensionToTypeMapping() );
			
			ExtensionToTypeMapping::const_iterator it = mapping.find( extension );
			
			if ( it != mapping.end() )
			{
				return FileSignature( '\?\?\?\?', it->second );
			}
		}
		
		return FileSignature( TextFileCreator(), 'TEXT' );
	}
	
}

