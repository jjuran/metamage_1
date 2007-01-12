/*	==========
 *	symlink.cc
 *	==========
 */

// Standard C
#include <errno.h>

// POSIX
#include "sys/stat.h"
#include "unistd.h"

// Nitrogen
#include "Nitrogen/Aliases.h"
#include "Nitrogen/Resources.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// POSeven
#include "POSeven/Errno.hh"

// OSErrno
#include "OSErrno/OSErrno.hh"

// Genie
#include "Genie/FileSystem/FSTree.hh"
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace P7 = POSeven;
	
	static const char* Basename( const char* pathname )
	{
		std::size_t length = std::strlen( pathname );
		
		const char* end = pathname + length;
		
		for ( const char* p = end - 1;  p >= pathname;  --p )
		{
			if ( *p == '/' )
			{
				return p + 1;
			}
		}
		
		return pathname;
	}
	
	static void CreateSymLink( const FSTreePtr& linkFile, const std::string& targetPath )
	{
		FSSpec linkSpec = linkFile->GetFSSpec();
		
		N::FSDirSpec linkParent = N::FSpGetParent( linkSpec );
		
		FSSpec linkParentSpec = NN::Convert< FSSpec >( linkParent );
		
		// Target pathname is resolved relative to the location of the link file
		FSTreePtr target = ResolvePathname( targetPath, FSTreeFromFSSpec( linkParentSpec ) );
		
		FSSpec targetSpec = target->GetFSSpec();
		
		FInfo fInfo = N::FSpGetFInfo( targetSpec );
		
		NN::Owned< N::AliasHandle > alias = N::NewAlias( linkSpec, targetSpec );
		
		N::FSpCreateResFile( linkSpec, fInfo.fdCreator, fInfo.fdType );
		
		fInfo = N::FSpGetFInfo( linkSpec );
		
		fInfo.fdFlags |= kIsAlias;
		
		N::FSpSetFInfo( linkSpec, fInfo );
		
		NN::Owned< N::ResFileRefNum > aliasResFile = N::FSpOpenResFile( linkSpec, N::fsRdWrPerm );
		
		(void) N::AddResource< N::rAliasType >( alias, N::ResID( 0 ), "\p" );
	}
	
	static int symlink( const char* target_path, const char* link_location )
	{
		try
		{
			FSTreePtr current = CurrentProcess().CurrentWorkingDirectory();
			
			FSTreePtr link = ResolvePathname( link_location, current );
			
			struct ::stat sb;
			
			try
			{
				// Stat the location.  Throws ENOENT if nonexistent.
				link->Stat( sb );
				
				bool isDir = sb.st_mode & S_IFDIR;
				
				if ( !isDir )
				{
					// Can't create link because a file is in the way.
					return CurrentProcess().SetErrno( EEXIST );
				}
				
				// Location is a dir; assume a filename.
				link = link->Lookup( Basename( target_path ) );
				
				// Stat the new location.  Throws ENOENT if nonexistent.
				link->Stat( sb );
				
				// The new location also exists; bail.
				return CurrentProcess().SetErrno( EEXIST );
			}
			catch ( const P7::Errno& errnum )
			{
				if ( errnum != ENOENT )  throw;
			}
			catch ( const N::FNFErr& fnf )
			{
			}
			catch ( const N::OSStatus& err )
			{
				if ( err.Get() != fnfErr )
				{
					throw;
				}
			}
			
			// If we got here, link is a valid location.
			
			CreateSymLink( link, target_path );
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( symlink );
	
}

