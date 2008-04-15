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
	
	static bool FileIsLocked( const FSSpec& file )
	{
		CInfoPBRec cInfo;
		
		N::FSpGetCatInfo( file, cInfo );
		
		bool locked = cInfo.hFileInfo.ioFlAttrib & kioFlAttribLockedMask;
		
		return locked;
	}
	
	class FileLockBypass
	{
		private:
			FSSpec  itsFileSpec;
			bool    itWasLocked;
		
		public:
			FileLockBypass( const FSSpec& file ) : itsFileSpec( file ),
			                                       itWasLocked( FileIsLocked( file ) )
			{
				if ( itWasLocked )
				{
					N::FSpRstFLock( file );
				}
			}
			
			~FileLockBypass()
			{
				if ( itWasLocked )
				{
					OSErr err = ::FSpSetFLock( &itsFileSpec );
				}
			}
			
			void SetFile( const FSSpec& file )
			{
				itsFileSpec = file;
			}
	};
	
	static void Rename( const FSSpec& srcFile, const FSSpec& destFile )
	{
		ASSERT( srcFile.vRefNum == destFile.vRefNum );
		ASSERT( srcFile.parID   == destFile.parID   );
		
		FileLockBypass lockBypass( srcFile );
		
		// Rename source to dest
		N::FSpRename( srcFile, destFile.name );
		
		lockBypass.SetFile( destFile );
	}
	
	static void MoveAndRename( const FSSpec& srcFile, const FSSpec& destFile )
	{
		// Darn, we have to move *and* rename.  Use MoreFiles.
		
		// destFolder is the parent of destFile.
		// It's semantically invalid (though it might work) to pass an
		// FSSpec with null fields to Mac OS, but MoreFiles won't do that.
		// (It breaks it into individual parts before passing them.)
			
		FSSpec destFolder;
		
		destFolder.vRefNum = destFile.vRefNum;
		destFolder.parID   = destFile.parID;
		
		destFolder.name[0] = '\0';
		
		FileLockBypass lockBypass( srcFile );
		
		N::ThrowOSStatus( ::FSpMoveRenameCompat( &srcFile, &destFolder, destFile.name ) );
		
		lockBypass.SetFile( destFile );
	}
	
	static int rename( const char* src, const char* dest )
	{
		SystemCallFrame frame( "rename" );
		
		try
		{
			FSTreePtr cwd = frame.Caller().GetCWD();
			
			FSSpec srcFile  = ResolvePathname( src,  cwd )->GetFSSpec();
			FSSpec destFile = ResolvePathname( dest, cwd )->GetFSSpec();
			
			if ( !io::item_exists( srcFile ) )
			{
				return frame.SetErrno( ENOENT );
			}
			
			// Do not resolve links
			
			N::Str63 requestedDestName = UntweakMacFilename( Basename( dest ) );
			
			// Can't move across volumes
			if ( srcFile.vRefNum != destFile.vRefNum )
			{
				return frame.SetErrno( EXDEV );
			}
			
			N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( srcFile.vRefNum );
			
			bool destExists = io::item_exists( destFile );
			
			bool srcIsDir  = io::directory_exists( srcFile  );
			bool destIsDir = io::directory_exists( destFile );
			
			if ( destExists  &&  srcIsDir != destIsDir )
			{
				return frame.SetErrno( destIsDir ? EISDIR : ENOTDIR );
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
				
				Rename( srcFile, destFile );
				
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
				// Same name, different dir; move only.
				N::FSpCatMove( srcFile, N::FSDirID( destFile.parID ) );
			}
			else
			{
				// Darn, we have to move *and* rename.
				MoveAndRename( srcFile, destFile );
			}
		}
		catch ( ... )
		{
			return frame.SetErrnoFromException();
		}
		
		return 0;
	}
	
	REGISTER_SYSTEM_CALL( rename );
	
}

