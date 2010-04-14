/*	======================
 *	Genie/FileSignature.cc
 *	======================
 */

#include "Genie/FileSignature.hh"

// Standard C++
#include <map>

// Nucleus
//#include "Nucleus/Convert.h"

// MacFeatures
#include "MacFeatures/Features.hh"

// Genie
//#include "Genie/Process.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	using MacFeatures::Has_OSXSystem;
	
	
	N::OSType TextFileCreator()
	{
		static bool runningOSX = Has_OSXSystem();
		
		return N::OSType( runningOSX ? '!Rch' : 'R*ch' );
		
		/*
		if ( const char* macEditorSignature = CurrentProcess().GetEnv( "MAC_EDITOR_SIGNATURE" ) )
		{
			try
			{
				return N::OSType( NN::Convert< N::FourCharCode, std::string >( macEditorSignature ).Get() );
			}
			catch ( ... )
			{
				// Probably not four characters long
			}
		}
		
		const N::OSType sigSimpleText = N::OSType( 'ttxt' );
		
		return sigSimpleText;
		*/
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
		
		// General
		{ "rsrc", 'rsrc', "resource file" },
		
		// Archive formats
		{ "mBin", 'BIN+', "MacBinary III+" " archive" },
		{ "jbin", 'BIN+', "MacBinary III+" " archive" },
		{ "mbin", 'mBIN', "MacBinary III"  " archive" },
		{ "mbi",  'mBIN', "MacBinary III"  " archive" },
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
		
		// PDF
		{ "pdf", 'PDF ', "PDF" " document" },
		
		// Image formats
		{ "gif",  'GIFf', "GIF"  " image" },
		{ "png",  'PNGf', "PNG"  " image" },
		{ "bmp",  'BMPf', "BMP"  " image" },
		{ "jpg",  'JPEG', "JPEG" " image" },
		{ "tiff", 'TIFF', "TIFF" " image" },
		{ "pict", 'PICT', "PICT" " image" },
		
		{ "mov", 'MooV', "QuickTime" " movie" },
		
		{ "mp3", 'MPEG', "MP3" " audio" },
		
		// Application-specific binary formats
		{ "lib", 'MPLF', "Metrowerks static library" },
	};
	
	struct FileSignature
	{
		::OSType creator;
		::OSType type;
	};
	
	const FileSignature gDefaultCreatorForTypeInput[] =
	{
		{ 'RSED', 'rsrc' },
		{ 'mBin', 'mBIN' },
		{ 'mBin', 'BIN+' },
		{ 'CARO', 'PDF ' },
		{ 'ogle', 'PICT' },
		{ 'ogle', 'PNGf' },
		{ 'ogle', 'GIFf' },
		{ 'ogle', 'TIFF' },
		{ 'ogle', 'JPEG' },
		{ 'TVOD', 'MPEG' },
		{ 'TVOD', 'MooV' },
		{ 'CWIE', 'MPLF' },
	};
	
	typedef std::map< std::string, N::FileSignature > ExtensionToTypeMapping;
	
	
	static inline bool operator==( const FileSignature& signature, ::OSType type )
	{
		return signature.type == type;
	}
	
	static const FileSignature* FindFileSignature( ::OSType type )
	{
		const FileSignature* begin = gDefaultCreatorForTypeInput;
		const FileSignature* end   = begin + sizeof gDefaultCreatorForTypeInput / sizeof gDefaultCreatorForTypeInput[0];
		
		const FileSignature* it = std::find( begin, end, type );
		
		return it != end ? it : NULL;
	}
	
	
	static N::OSType GetCreatorForType( N::OSType type )
	{
		if ( const FileSignature* it = FindFileSignature( type ) )
		{
			return N::OSType( it->creator );
		}
		
		return N::OSType( '\?\?\?\?' );
	}
	
	static ExtensionToTypeMapping BuildExtensionToTypeMapping()
	{
		ExtensionToTypeMapping result;
		
		const std::size_t extensionCount = sizeof gExtensionToTypeMappingInput / sizeof (ExtensionToTypeRecord);
		
		// Would be nice to use std::for_each() but it's just too much of a PITA.
		for ( const ExtensionToTypeRecord* p = gExtensionToTypeMappingInput;  p < gExtensionToTypeMappingInput + extensionCount;  ++p )
		{
			// ASSERT( p->extension != NULL );
			N::OSType type = N::OSType( p->type );
			
			N::FileSignature signature( GetCreatorForType( type ), type );
			
			result[ std::string( p->extension ) ] = signature;
		}
		
		return result;
	}
	
	static const ExtensionToTypeMapping& GetExtensionToTypeMapping()
	{
		static ExtensionToTypeMapping mapping( BuildExtensionToTypeMapping() );
		
		return mapping;
	}
	
	N::FileSignature PickFileSignatureForName( const std::string& name )
	{
		std::size_t dot = name.find_last_of( "." );
		
		if ( dot != name.npos )
		{
			std::string extension = name.substr( dot + 1, name.npos );
			
			const ExtensionToTypeMapping& mapping( GetExtensionToTypeMapping() );
			
			ExtensionToTypeMapping::const_iterator it = mapping.find( extension );
			
			if ( it != mapping.end() )
			{
				return it->second;
			}
		}
		
		return N::FileSignature( TextFileCreator(), N::OSType( 'TEXT' ) );
	}
	
}

