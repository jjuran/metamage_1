/*	========
 *	cpres.cc
 *	========
 */

// Nitrogen
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/Resources.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"
#include "Utilities/Saved.h"

// Orion
#include "Orion/Main.hh"
#include "Orion/StandardIO.hh"
#include "SystemCalls.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace O = Orion;


static int TryResCopy( const FSSpec& source, N::ResFileRefNum dest );
	
int O::Main( int argc, char const *const argv[] )
{
	int fail = 0;
	
	// Check for sufficient number of args
	if ( argc < 3 )
	{
		Io::Err << "cpres: missing "
		        << ( argc == 1 ? "file arguments"
		                       : "destination file")
		        << "\n";
		
		return 1;
	}
	
	// Last arg should be the destination file.
	FSSpec dest;
	
	try
	{
		dest = Path2FSS( argv[ argc - 1 ] );
	}
	catch ( ... )
	{
		Io::Err << "cpres: last argument (" << argv[ argc - 1 ] << ") is not a file.\n";
		return 1;
	}
	
	::FSpCreateResFile( &dest, 'RSED', 'rsrc', smRoman );
	
	NN::Owned< N::ResFileRefNum > resFileH( N::FSpOpenResFile( dest, fsRdWrPerm ) );
	
	// Try to copy each file.  Return whether any errors occurred.
	for ( int index = 1;  index < argc - 1;  ++index )
	{
		try
		{
			FSSpec source = Path2FSS( argv[ index ] );
			fail += TryResCopy( source, resFileH );
		}
		catch ( N::OSStatus err )
		{
			fail++;
			Io::Err << "OSStatus " << err << " copying from " << argv[ index ] << " to " << dest.name << ".\n";
		}
	}
	
	return fail;
}

int TryResCopy( const FSSpec& source, N::ResFileRefNum destRes )
{
	if ( N::FSpTestDirectoryExists( source ) )
	{
		// Source item is a directory.
		Io::Err << "cpres: " << source.name << ": omitting directory\n";
		return 1;
	}
	
	NN::Owned< N::ResFileRefNum > sourceRes( N::FSpOpenResFile( source, fsRdPerm ) );
	
	int types = N::Count1Types();
	
	for ( int iType = 1;  iType <= types;  ++iType )
	{
		N::ResType type = N::Get1IndType( iType );
		int rsrcs = N::Count1Resources( type );
		
		for ( int iRsrc = 1;  iRsrc <= rsrcs;  ++iRsrc )
		{
			Handle h = N::Get1IndResource( type, iRsrc );
			::HNoPurge( h );
			
			N::Saved< N::ResFile_Value > savedResFile( destRes );
			
			N::GetResInfo_Result resInfo = N::GetResInfo( h );
			
			try
			{
				Handle existing = N::Get1Resource( resInfo.type, resInfo.id );
				
				if ( existing )
				{
					::RemoveResource( existing );
				}
			}
			catch ( N::ResNotFound ) {}  // Okay, resource didn't exist in dest file
			
			N::AddResource( N::DetachResource( h ), resInfo );
		}
	}
	
	return 0;
}

