/*
	Genie/FS/HFS/Rename.cc
	----------------------
*/

#include "Genie/FS/HFS/Rename.hh"

// MoreFiles
#include "MoreFiles/MoreFilesExtras.h"

// Debug
#include "debug/assert.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/replaced_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Files.hh"

// vfs
#include "vfs/functions/file-tests.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// MacIO
#include "MacIO/GetCatInfo_Sync.hh"

// Genie
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/FSTree.hh"
#include "Genie/FS/HFS/hashed_long_name.hh"
#include "Genie/FS/HFS/LongName.hh"
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static bool FileIsLocked( const FSSpec& file )
	{
		CInfoPBRec cInfo = { 0 };
		
		MacIO::GetCatInfo< FNF_Throws >( cInfo, file );
		
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
	
	static OSErr ForceRenameItem( const FSSpec& srcFile, const FSSpec& destFile )
	{
		ASSERT( srcFile.vRefNum == destFile.vRefNum );
		ASSERT( srcFile.parID   == destFile.parID   );
		
		FileLockBypass lockBypass( srcFile );
		
		// Rename source to dest
		OSErr err = ::FSpRename( &srcFile, destFile.name );
		
		if ( err == dupFNErr )
		{
			err = ::FSpDelete( &destFile );
			
			if ( err != noErr )
			{
				return err;
			}
			
			err = ::FSpRename( &srcFile, destFile.name );
		}
		
		Mac::ThrowOSStatus( err );
		
		lockBypass.SetFile( destFile );
		
		return noErr;
	}
	
	
	static inline plus::string slashes_from_colons( const plus::string& unix_name )
	{
		return plus::replaced_string( unix_name, ':', '/' );
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
		
		Mac::ThrowOSStatus( ::FSpMoveRenameCompat( &srcFile, &destFolder, destFile.name ) );
		
		lockBypass.SetFile( destFile );
	}
	
	
	void Rename_HFS( const FSSpec& srcFileSpec, const vfs::node& destFile )
	{
		if ( !io::item_exists( srcFileSpec ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		bool destExists = exists( destFile );
		
		bool srcIsDir  = io::directory_exists( srcFileSpec );
		bool destIsDir = is_directory( destFile );
		
		if ( destExists  &&  srcIsDir != destIsDir )
		{
			p7::throw_errno( destIsDir ? EISDIR : ENOTDIR );
		}
		
		FSSpec destFileSpec = GetFSSpecFromFSTree( destFile );
		
		// Can't move across volumes
		if ( srcFileSpec.vRefNum != destFileSpec.vRefNum )
		{
			p7::throw_errno( EXDEV );
		}
		
		N::FSVolumeRefNum vRefNum = N::FSVolumeRefNum( srcFileSpec.vRefNum );
		
		const plus::string& destName = slashes_from_colons( plus::mac_from_utf8( destFile.name() ) );
		
		const char* dest = destName.begin();
		
		const size_t n_dest = destName.size();
		
		const unsigned char* src = srcFileSpec.name;
		
		const uint8_t n_src = *src++;
		
		const bool keeping_name = n_src == n_dest  &&  memcmp( src, dest, n_dest ) == 0;
		
		if ( srcFileSpec.parID == destFileSpec.parID )
		{
			// Same dir.
			
			if ( keeping_name )
			{
				// Identical names in the same directory.  Nothing to do.
				return;
			}
			
			OSErr deleteErr = ForceRenameItem( srcFileSpec, destFileSpec );
			
			if ( deleteErr == fBsyErr  &&  destIsDir )
			{
				p7::throw_errno( ENOTEMPTY );
			}
			
			SetLongName( destFileSpec, destName );
			
			// And we're done
			return;
		}
		
		if ( destExists )
		{
			// Can't be the same file, because it's in a different directory.
			
			N::FSpDelete( destFileSpec );
		}
		
		if ( keeping_name )
		{
			// Same name, different dir; move only.
			N::FSpCatMove( srcFileSpec, N::FSDirID( destFileSpec.parID ) );
		}
		else
		{
			// Darn, we have to move *and* rename.
			MoveAndRename( srcFileSpec, destFileSpec );
			
			SetLongName( destFileSpec, destName );
		}
	}
	
}

