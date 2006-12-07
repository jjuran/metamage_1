/*	=========
 *	rename.cc
 *	=========
 */

// Mac OS Universal Interfaces
#include <OSUtils.h>
#include <TextUtils.h>

// Standard C
#include <errno.h>
#include <string.h>

// Standard C++
#include <algorithm>
#include <string>

// POSIX
#include "unistd.h"

// MoreFiles
#include "MoreFilesExtras.h"

// Nitrogen / Carbon
#include "Nitrogen/OSStatus.h"

// Nitrogen Extras / Utilities
#include "Utilities/Files.h"

// Genie
#include "Genie/pathnames.hh"
#include "Genie/Process.hh"
#include "Genie/SystemCallRegistry.hh"
#include "Genie/SystemCalls.hh"
#include "Genie/Yield.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	static const char* Basename( const char* path )
	{
		const char* slash = std::strrchr( path, '/' );
		
		if ( slash == NULL )
		{
			return path;
		}
		
		return slash + 1;
	}
	
	static std::string UntweakMacFilename( std::string name )
	{
		std::replace( name.begin(), name.end(), ':', '/' );
		
		return name;
	}
	
	static int rename( const char* src, const char* dest )
	{
		try
		{
			const N::FSDirSpec cwd = CurrentProcess().CurrentDirectory();
			
			FSSpec srcFile  = ResolveUnixPathname( src,  cwd );
			FSSpec destFile = ResolveUnixPathname( dest, cwd );
			
			N::Str63 requestedDestName = UntweakMacFilename( Basename( dest ) );
			
			// Can't move across volumes
			if ( srcFile.vRefNum != destFile.vRefNum )
			{
				return CurrentProcess().SetErrno( EXDEV );
			}
			
			N::FSVolumeRefNum vRefNum = srcFile.vRefNum;
			
			bool destExists = false;
			
			try
			{
				CInfoPBRec paramBlock;
				
				N::FSpGetCatInfo( destFile, paramBlock );
				
				bool isDir = N::PBTestIsDirectory( paramBlock );
				
				if ( isDir )
				{
					// Directory specified -- look within
					N::FSDirID dirID = paramBlock.dirInfo.ioDrDirID;
					//destFile = N::FSMakeFSSpec( vRefNum, dirID, srcFile.name );
					destFile = NN::Convert< N::FSDirSpec >( destFile ) & srcFile.name;
					
					N::FSpGetCatInfo( destFile, paramBlock );
					isDir = N::PBTestIsDirectory( paramBlock );
					
					if ( isDir )
					{
						// Also a directory -- can't replace it.
						return CurrentProcess().SetErrno( EISDIR );
					}
				}
				
				// destFile is now the item we're going to replace
				destExists = true;
			}
			catch ( const N::FNFErr& )
			{
				// destFile is absent
			}
			
			bool srcAndDestNamesEqual = std::equal( srcFile.name,
			                                        srcFile.name + 1 + srcFile.name[0],
			                                        destFile.name );
			
			if ( srcFile.parID == destFile.parID )
			{
				// Same dir.
				
				bool sameFile = ::EqualString( srcFile.name, destFile.name, false, true );
				
				if ( sameFile )
				{
					// Could be a case change.
					bool caseChanged = !std::equal( destFile.name,
					                                destFile.name + 1 + destFile.name[0],
					                                static_cast< const unsigned char* >( requestedDestName ) );
					
					if ( !caseChanged )
					{
						// Source and dest are the same file, and we're not changing case
						return 0;
					}
					
					// sameFile now implies caseChanged
				}
				
				// Don't delete the dest file if it's really the same file!
				if ( destExists && !sameFile )
				{
					// Delete existing dest file
					
					N::FSpDelete( destFile );
				}
				
				// Rename source to dest
				//N::FSpRename( srcFile, requestedDestName );
				N::ThrowOSStatus( ::FSpRename( &srcFile, requestedDestName ) );
				
				// And we're done
				return 0;
			}
			
			if ( destExists )
			{
				// Can't be the same file, because it's in a different directory.
				
				N::FSpDelete( destFile );
			}
			
			if ( srcAndDestNamesEqual )
			{
				// Same name, different dir.
				
				// Move source to dest
				N::FSpCatMove( srcFile, destFile.parID );
				
				// And we're done
				return 0;
			}
			
			// Darn, we have to move *and* rename.  Use MoreFiles.
			
			N::Str255 name = destFile.name;
			destFile.name[0] = '\0';
			N::ThrowOSStatus( ::FSpMoveRenameCompat( &srcFile, &destFile, name ) );
		}
		catch ( ... )
		{
			return GetErrnoFromExceptionInSystemCall();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( rename );
	
}

