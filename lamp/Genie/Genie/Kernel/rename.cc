/*	=========
 *	rename.cc
 *	=========
 */

// Mac OS Universal Interfaces
#include <OSUtils.h>
#include <TextUtils.h>

// Standard C++
#include <algorithm>
#include <string>

// Standard C
#include <errno.h>
#include <string.h>

// POSIX
#include <unistd.h>

// MoreFiles
#include "MoreFilesExtras.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"

// Genie
#include "Genie/FileSystem/ResolvePathname.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"


namespace Genie
{
	
	DECLARE_MODULE_INIT( Kernel_rename )
	DEFINE_MODULE_INIT(  Kernel_rename )
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	using namespace io::path_descent_operators;
	
	
	static const char* Basename( const char* path )
	{
		const char* slash = std::strrchr( path, '/' );
		
		if ( slash == NULL )
		{
			return path;
		}
		
		return slash + 1;
	}
	
	inline std::string UntweakMacFilename( std::string name )
	{
		std::replace( name.begin(), name.end(), ':', '/' );
		
		return name;
	}
	
	static bool NamesAreSame( ConstStr63Param a, ConstStr63Param b )
	{
		// b may be a null string to indicate a will be reused, so they're the same
		return b[0] == 0  ||  std::equal( a,
		                                  a + 1 + a[0],
		                                  b );
	}
	
	static int rename( const char* src, const char* dest )
	{
		SystemCallFrame frame( "rename" );
		
		try
		{
			FSTreePtr cwd = frame.Caller().GetCWD();
			
			FSSpec srcFile  = ResolvePathname( src,  cwd )->GetFSSpec();
			FSSpec destFile = ResolvePathname( dest, cwd )->GetFSSpec();
			
			// Do not resolve links
			
			N::Str63 requestedDestName = UntweakMacFilename( Basename( dest ) );
			
			// Can't move across volumes
			if ( srcFile.vRefNum != destFile.vRefNum )
			{
				return frame.SetErrno( EXDEV );
			}
			
			N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( srcFile.vRefNum );
			
			bool destExists = false;
			
			try
			{
				CInfoPBRec cInfo;
				
				N::FSpGetCatInfo( destFile, cInfo );
				
				bool isDir = io::item_is_directory( cInfo );
				
				if ( isDir )
				{
					// Directory specified -- look within
					//N::FSDirID dirID = N::FSDirID( cInfo.dirInfo.ioDrDirID );
					//destFile = N::FSMakeFSSpec( vRefNum, dirID, srcFile.name );
					destFile = destFile / srcFile.name;
					
					requestedDestName[0] = 0;  // signal that source name will be used
					
					N::FSpGetCatInfo( destFile, cInfo );
					
					if ( io::item_is_directory( cInfo ) )
					{
						// Also a directory -- can't replace it.
						return frame.SetErrno( EISDIR );
					}
				}
				
				// destFile is now the item we're going to replace
				destExists = true;
			}
			catch ( const N::FNFErr& err )
			{
			#ifdef __MWERKS__
				
				if ( err.Get() != fnfErr )
				{
					throw;
				}
				
			#endif
				
				// destFile is absent
			}
			
			if ( srcFile.parID == destFile.parID )
			{
				// Same dir.
				
				bool sameFile = ::EqualString( srcFile.name, destFile.name, false, true );
				
				if ( sameFile )
				{
					// Could be a case change, or not
					if ( NamesAreSame( destFile.name, requestedDestName ) )
					{
						// Source and dest are the same file, and we're not changing case
						return 0;
					}
					
					// sameFile now implies case has changed
				}
				
				// Don't delete the dest file if it's really the same file!
				if ( destExists && !sameFile )
				{
					// Delete existing dest file
					
					N::FSpDelete( destFile );
				}
				
				// Rename source to dest
				N::FSpRename( srcFile, requestedDestName );
				
				// And we're done
				return 0;
			}
			
			if ( destExists )
			{
				// Can't be the same file, because it's in a different directory.
				
				N::FSpDelete( destFile );
			}
			
			if ( NamesAreSame( srcFile.name, requestedDestName ) )
			{
				// Same name, different dir.
				
				// Move source to dest
				N::FSpCatMove( srcFile, N::FSDirID( destFile.parID ) );
				
				// And we're done
				return 0;
			}
			
			// Darn, we have to move *and* rename.  Use MoreFiles.
			
			destFile.name[0] = '\0';
			
			N::ThrowOSStatus( ::FSpMoveRenameCompat( &srcFile, &destFile, requestedDestName ) );
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( rename );
	
}

