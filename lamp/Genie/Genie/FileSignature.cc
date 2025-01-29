/*
	FileSignature.cc
	----------------
*/

#include "Genie/FileSignature.hh"

// Standard C
#include <string.h>

// mac-sys-utils
#include "mac_sys/has/Darwin_kernel.hh"

// gear
#include "gear/find.hh"


namespace Genie
{

enum
{
	sigBBEdit       = 'R*ch',
	sigTextWrangler = '!Rch',
};

OSType gTextFileCreator = mac::sys::has_Darwin_kernel() ? sigTextWrangler
                                                        : sigBBEdit;


struct ExtensionToTypeRecord
{
	const char* extension;
	::OSType type;
	const char* comment;
};

const ExtensionToTypeRecord gExtensionToTypeMappingInput[] =
{
	/*
		Notes
		-----
		.sit is ambiguous: 'SIT!', 'SITD', and 'SIT5' are all possible
		translations.  My copy of Internet Config uses 'SITD'.
		
		.bin is ambiguous, used for MacBinary and disk images.  We'll
		need to have a generic binary type and a means of autodetection.
		
		UPDATE:
		To avoid pain, we're going with .mbin / 'mBIN' for new archives.
	*/
	
	// General
	{ "rsrc", 'rsrc', "resource file" },
	
	// Archive formats
	{ "mBin", 'BIN+', "MacBinary III+" " archive" },
	{ "mbim", 'BIN+', "MacBinary III+" " archive" },
	{ "mbin", 'mBIN', "MacBinary III"  " archive" },
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
	::OSType type;
	::OSType creator;
};

const FileSignature gDefaultCreatorForTypeInput[] =
{
	{ 'rsrc', 'RSED' },
	{ 'mBIN', 'mBin' },
	{ 'BIN+', 'mBin' },
	{ 'PDF ', 'CARO' },
	{ 'PICT', 'ogle' },
	{ 'PNGf', 'ogle' },
	{ 'GIFf', 'ogle' },
	{ 'TIFF', 'ogle' },
	{ 'JPEG', 'ogle' },
	{ 'MPEG', 'TVOD' },
	{ 'MooV', 'TVOD' },
	{ 'MPLF', 'CWIE' },
};


static inline
const ExtensionToTypeRecord* FindExtensionToTypeRecord( const char* extension )
{
	const ExtensionToTypeRecord* begin = gExtensionToTypeMappingInput;
	const ExtensionToTypeRecord* end   = begin + sizeof gExtensionToTypeMappingInput / sizeof gExtensionToTypeMappingInput[0];
	
	for ( const ExtensionToTypeRecord* it = begin;  it != end;  ++it )
	{
		if ( strcmp( it->extension, extension ) == 0 )
		{
			return it;
		}
	}
	
	return NULL;
}

static
const FileSignature* FindFileSignature( ::OSType type )
{
	const FileSignature* begin = gDefaultCreatorForTypeInput;
	const FileSignature* end   = begin + sizeof gDefaultCreatorForTypeInput / sizeof gDefaultCreatorForTypeInput[0];
	
	for ( const FileSignature* it = begin;  it != end;  ++it )
	{
		if ( it->type == type )
		{
			return it;
		}
	}
	
	return NULL;
}


static inline
Mac::FSCreator GetCreatorForType( Mac::FSType type )
{
	if ( const FileSignature* it = FindFileSignature( type ) )
	{
		return Mac::FSCreator( it->creator );
	}
	
	return Mac::kUnknownFSCreator;  // '????'
}

Mac::FSSignature PickFileSignatureForName( const char*  name,
                                           unsigned     length )
{
	if ( const char* dot = gear::find_last_match( name, length, '.' ) )
	{
		const char* extension = dot + 1;
		
		if ( const ExtensionToTypeRecord* it = FindExtensionToTypeRecord( extension ) )
		{
			Mac::FSType type = Mac::FSType( it->type );
			
			Mac::FSSignature signature( type, GetCreatorForType( type ) );
			
			return signature;
		}
	}
	
	return Mac::FSSignature( Mac::FSType( 'TEXT' ), TextFileCreator() );
}

}
