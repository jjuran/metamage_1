/*	========
 *	cpres.cc
 *	========
 */

// Nucleus
#include "Nucleus/Saved.h"

// Nitrogen
#include "Nitrogen/OSStatus.hh"
#include "Nitrogen/Resources.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// Divergence
#include "Divergence/Utilities.hh"

// Orion
#include "Orion/get_options.hh"
#include "Orion/Main.hh"


namespace N = Nitrogen;
namespace NN = Nucleus;
namespace Div = Divergence;


namespace tool
{
	
	namespace o = orion;
	
	
	static bool globally_using_data_fork = false;
	
	
	static NN::Owned< N::ResFileRefNum > open_res_file_from_data_fork( const FSSpec&   filespec,
	                                                                   N::FSIOPermssn  perm )
	{
		FSRef fileref = NN::Convert< FSRef >( filespec );
		
		return N::FSOpenResourceFile( fileref, N::UniString(), perm );
	}
	
	static NN::Owned< N::ResFileRefNum > open_res_file( const FSSpec&   filespec,
	                                                    N::FSIOPermssn  perm )
	{
		try
		{
			if ( TARGET_API_MAC_CARBON && globally_using_data_fork )
			{
				return open_res_file_from_data_fork( filespec, perm );
			}
		}
		catch ( ... )
		{
		}
		
		return N::FSpOpenResFile( filespec, perm );
	}
	
	
	static int TryResCopy( const FSSpec& source, N::ResFileRefNum destRes )
	{
		if ( io::directory_exists( source ) )
		{
			std::string name = NN::Convert< std::string >( source.name );
			
			// Source item is a directory.
			std::fprintf( stderr, "cpres: %s: omitting directory\n", name.c_str() );
			
			return 1;
		}
		
		NN::Owned< N::ResFileRefNum > sourceRes( open_res_file( source, N::fsRdPerm ) );
		
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
				catch ( const N::OSStatus& err )
				{
					if ( err != resNotFound )
					{
						throw;
					}
					
					// Okay, resource didn't exist in dest file
				}
				
				N::AddResource( N::DetachResource( h ), resInfo );
			}
		}
		
		return 0;
	}
	
	
	int Main( int argc, iota::argv_t argv )
	{
		NN::RegisterExceptionConversion< NN::Exception, N::OSStatus >();
		
		o::bind_option_to_variable( "--data", globally_using_data_fork );
		
		o::get_options( argc, argv );
		
		char const *const *freeArgs = o::free_arguments();
		
		std::size_t n_args = o::free_argument_count();
		
		if ( globally_using_data_fork  &&  ( !TARGET_API_MAC_CARBON || ::FSOpenResourceFile == NULL ) )
		{
			std::fprintf( stderr, "cpres: FSOpenResourceFile() unavailable for data fork\n" );
			
			return 2;
		}
		
		int fail = 0;
		
		// Check for sufficient number of args
		if ( n_args < 2 )
		{
			std::fprintf( stderr, "cpres: missing %s\n", (n_args == 0) ? "file arguments"
			                                                           : "destination file" );
			
			return 1;
		}
		
		// Last arg should be the destination file.
		FSSpec dest;
		
		try
		{
			dest = Div::ResolvePathToFSSpec( freeArgs[ n_args - 1 ] );
		}
		catch ( ... )
		{
			std::fprintf( stderr, "cpres: last argument (%s) is not a file.\n", freeArgs[ n_args - 1 ] );
			
			return 1;
		}
		
		if ( TARGET_API_MAC_CARBON && globally_using_data_fork )
		{
			FSSpec parent_spec = NN::Convert< FSSpec >( io::get_preceding_directory( dest ) );
			
			FSRef parent_ref = NN::Convert< FSRef >( parent_spec );
			
			N::UniString name( dest.name + 1, dest.name + 1 + dest.name[0] );
			
			N::ThrowOSStatus( ::FSCreateResourceFile( &parent_ref,
			                                          name.size(),
			                                          name.data(),
			                                          FSCatalogInfoBitmap(),
			                                          NULL,
			                                          0,
			                                          NULL,
			                                          NULL,
			                                          NULL ) );
		}
		else
		{
			::FSpCreateResFile( &dest, 'RSED', 'rsrc', smRoman );
		}
		
		NN::Owned< N::ResFileRefNum > resFileH( open_res_file( dest, N::fsRdWrPerm ) );
		
		// Try to copy each file.  Return whether any errors occurred.
		for ( int index = 0;  index < n_args - 1;  ++index )
		{
			try
			{
				FSSpec source = Div::ResolvePathToFSSpec( freeArgs[ index ] );
				
				fail += TryResCopy( source, resFileH );
			}
			catch ( const N::OSStatus& err )
			{
				++fail;
				
				std::string destName = NN::Convert< std::string >( dest.name );
				
				std::fprintf( stderr, "OSStatus %d copying from %s to %s.\n",
				                                err.Get(),      freeArgs[ index ],
				                                                      destName.c_str() );
			}
		}
		
		return fail;
	}
	
}

