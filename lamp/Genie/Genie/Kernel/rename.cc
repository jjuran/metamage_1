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
#include <sys/stat.h>
#include <unistd.h>

// MoreFiles
#include "MoreFilesExtras.h"

// Nitrogen
#include "Nitrogen/OSStatus.h"
#include "Nitrogen/Str.h"

// POSeven
#include "POSeven/Errno.hh"

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
	namespace p7 = poseven;
	
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
	
	static void SetLongName( const FSSpec& item, const char* pathname )
	{
		const char* name = Basename( pathname );
		
		std::size_t length = std::strlen( name );
		
		if ( length > 31 )
		{
			N::FSpDTSetComment( item, std::string( name, length ) );
		}
	}
	
	static FSSpec GetFSSpecForRenameDestination( const FSTreePtr& file, const char* path )
	{
		struct ::stat stat_buffer;
		
		// Throws ENOENT if parent doesn't exist
		file->Parent()->Stat( stat_buffer );
		
		if ( stat_buffer.st_dev <= 0 )
		{
			// Not FSSpecified
			p7::throw_errno( EXDEV );
		}
		
		FSSpec result;
		
		// vRefNum and dir ID from parent
		result.vRefNum = -stat_buffer.st_dev;
		result.parID   =  stat_buffer.st_ino;
		
		// The requested name (not the on-disk name, in the event of case conflict)
		N::CopyToPascalString( UntweakMacFilename( Basename( path ) ),
		                       result.name,
		                       sizeof result.name - 1 );
		
		return result;
	}
	
	static int rename( const char* src, const char* dest )
	{
		SystemCallFrame frame( "rename" );
		
		try
		{
			FSTreePtr cwd = frame.Caller().GetCWD();
			
			FSTreePtr srcFile  = ResolvePathname( src,   cwd );
			FSTreePtr destFile = ResolvePathname( dest,  cwd );
			
			// Do not resolve links
			
			if ( !srcFile->Exists() )
			{
				return frame.SetErrno( ENOENT );
			}
			
			bool destExists = destFile->Exists();
			
			bool srcIsDir  = srcFile ->IsDirectory();
			bool destIsDir = destFile->IsDirectory();
			
			if ( destExists  &&  srcIsDir != destIsDir )
			{
				return frame.SetErrno( destIsDir ? EISDIR : ENOTDIR );
			}
			
			FSSpec srcFileSpec = srcFile->GetFSSpec( false );
			FSSpec destFileSpec;
			
			try
			{
				destFileSpec = destFile->GetFSSpec( !destExists );
			}
			catch ( ... )
			{
				// Case-sensitivity conflict
				destFileSpec = GetFSSpecForRenameDestination( destFile, dest );
			}
			
			// Can't move across volumes
			if ( srcFileSpec.vRefNum != destFileSpec.vRefNum )
			{
				return frame.SetErrno( EXDEV );
			}
			
			N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( srcFileSpec.vRefNum );
			
			bool sameName = NamesAreSame( srcFileSpec.name, destFileSpec.name );
			
			if ( srcFileSpec.parID == destFileSpec.parID )
			{
				// Same dir.
				
				if ( sameName )
				{
					// Identical names in the same directory.  Nothing to do.
					return 0;
				}
				
				// destExists is false for case changes
				if ( destExists )
				{
					// Delete existing dest file
					N::FSpDelete( destFileSpec );
				}
				
				Rename( srcFileSpec, destFileSpec );
				
				SetLongName( destFileSpec, dest );
				
				// And we're done
				return 0;
			}
			
			if ( destExists )
			{
				// Can't be the same file, because it's in a different directory.
				
				N::FSpDelete( destFileSpec );
			}
			
			if ( sameName )
			{
				// Same name, different dir; move only.
				N::FSpCatMove( srcFileSpec, N::FSDirID( destFileSpec.parID ) );
			}
			else
			{
				// Darn, we have to move *and* rename.
				MoveAndRename( srcFileSpec, destFileSpec );
				
				SetLongName( destFileSpec, dest );
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

