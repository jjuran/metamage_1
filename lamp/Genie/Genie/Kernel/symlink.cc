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
	
	DECLARE_MODULE_INIT( Kernel_symlink )
	DEFINE_MODULE_INIT(  Kernel_symlink )
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	namespace p7 = poseven;
	
	
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
	
	static N::FileSignature GetFileSignatureForAlias( const FSSpec& item )
	{
		if ( io::directory_exists( item ) )
		{
			return N::FileSignature( N::OSType( 'MACS'                    ),
			                         N::OSType( kContainerFolderAliasType ) );
		}
		
		FInfo fInfo = N::FSpGetFInfo( item );
		
		return N::FileSignature( fInfo );
	}
	
	static void CreateSymLink( const FSTreePtr& linkFile, const std::string& targetPath )
	{
		FSSpec linkSpec = linkFile->GetFSSpec();
		
		N::FSDirSpec linkParent = io::get_preceding_directory( linkSpec );
		
		FSSpec linkParentSpec = NN::Convert< FSSpec >( linkParent );
		
		// Target pathname is resolved relative to the location of the link file
		FSTreePtr target = ResolvePathname( targetPath, FSTreeFromFSSpec( linkParentSpec ) );
		
		// Do not resolve links -- if the target of this link is another symlink, so be it
		
		FSSpec targetSpec = target->GetFSSpec();
		
		NN::Owned< N::AliasHandle > alias = N::NewAlias( linkSpec, targetSpec );
		
		N::FileSignature signature = GetFileSignatureForAlias( targetSpec );
		
		N::FSpCreateResFile( linkSpec, signature );
		
		FInfo linkFInfo = N::FSpGetFInfo( linkSpec );
		
		linkFInfo.fdFlags |= kIsAlias;
		
		N::FSpSetFInfo( linkSpec, linkFInfo );
		
		NN::Owned< N::ResFileRefNum > aliasResFile = N::FSpOpenResFile( linkSpec, N::fsRdWrPerm );
		
		(void) N::AddResource< N::rAliasType >( alias, N::ResID( 0 ), "\p" );
	}
	
	static int symlink( const char* target_path, const char* link_location )
	{
		SystemCallFrame frame( "symlink" );
		
		try
		{
			FSTreePtr current = frame.Caller().GetCWD();
			
			FSTreePtr link = ResolvePathname( link_location, current );
			
			// Do not resolve links.  If there's a symlink in this location, throw EEXIST.
			
			struct ::stat location_status;
			
			try
			{
				// Stat the location.  Throws ENOENT if nonexistent.
				link->Stat( location_status );
				
				// The new location also exists; bail.
				return frame.SetErrno( EEXIST );
			}
			catch ( const p7::errno_t& errnum )
			{
				if ( errnum != ENOENT )  throw;
			}
			catch ( const N::FNFErr& err )
			{
			#ifdef __MWERKS__
				
				if ( err.Get() != fnfErr )
				{
					throw;
				}
				
			#endif
			}
			
			// If we got here, link is a valid location.
			
			CreateSymLink( link, target_path );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( symlink );
	
	static ssize_t readlink_k( const char *path, char *buffer, size_t buffer_size )
	{
		SystemCallFrame frame( "readlink_k" );
		
		try
		{
			FSTreePtr current = frame.Caller().GetCWD();
			
			FSTreePtr link = ResolvePathname( path, current );
			
			// Do not resolve links -- we want the target even if it's another symlink
			
			std::string linkPath = link->ReadLink();
			
			buffer_size = std::min( buffer_size, linkPath.size() );
			
			std::copy( linkPath.begin(),
			           linkPath.begin() + buffer_size,
			           buffer );
			
			return linkPath.size();
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
	}
	
	REGISTER_SYSTEM_CALL( readlink_k );
	
}

