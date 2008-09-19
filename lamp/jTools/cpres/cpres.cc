/*	========
 *	cpres.cc
 *	========
 */

// Nucleus
#include "Nucleus/Saved.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/Resources.h"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/Main.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace Div = Divergence;


namespace tool
{
	
	static int TryResCopy( const FSSpec& source, N::ResFileRefNum destRes )
	{
		if ( io::directory_exists( source ) )
		{
			std::string name = NN::Convert< std::string >( source.name );
			
			// Source item is a directory.
			std::fprintf( stderr, "cpres: %s: omitting directory\n", name.c_str() );
			
			return 1;
		}
		
		using N::fsRdPerm;
		
		NN::Owned< N::ResFileRefNum > sourceRes( N::FSpOpenResFile( source, fsRdPerm ) );
		
		int types = N::Count1Types();
		
		for ( int iType = 1;  iType <= types;  ++iType )
		{
			N::ResType type = N::Get1IndType( iType );
			int rsrcs = N::Count1Resources( type );
			
			for ( int iRsrc = 1;  iRsrc <= rsrcs;  ++iRsrc )
			{
				N::Handle h = N::Get1IndResource( type, iRsrc );
				
				::HNoPurge( h );
				
				NN::Saved< N::ResFile_Value > savedResFile( destRes );
				
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
	
	int Main( int argc, iota::argv_t argv )
	{
		int fail = 0;
		
		// Check for sufficient number of args
		if ( argc < 3 )
		{
			std::fprintf( stderr, "cpres: missing %s\n", (argc == 1) ? "file arguments"
			                                                         : "destination file" );
			
			return 1;
		}
		
		// Last arg should be the destination file.
		FSSpec dest;
		
		try
		{
			dest = Div::ResolvePathToFSSpec( argv[ argc - 1 ] );
		}
		catch ( ... )
		{
			std::fprintf( stderr, "cpres: last argument (%s) is not a file.\n", argv[ argc - 1 ] );
			
			return 1;
		}
		
		::FSpCreateResFile( &dest, 'RSED', 'rsrc', smRoman );
		
		using N::fsRdWrPerm;
		
		NN::Owned< N::ResFileRefNum > resFileH( N::FSpOpenResFile( dest, fsRdWrPerm ) );
		
		// Try to copy each file.  Return whether any errors occurred.
		for ( int index = 1;  index < argc - 1;  ++index )
		{
			try
			{
				FSSpec source = Div::ResolvePathToFSSpec( argv[ index ] );
				
				fail += TryResCopy( source, resFileH );
			}
			catch ( const N::OSStatus& err )
			{
				++fail;
				
				std::string destName = NN::Convert< std::string >( dest.name );
				
				std::fprintf( stderr, "OSStatus %d copying from %s to %s.\n",
				                                err.Get(),      argv[ index ],
				                                                      destName.c_str() );
			}
		}
		
		return fail;
	}
	
}

