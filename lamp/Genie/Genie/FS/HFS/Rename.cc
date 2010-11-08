/*
	Genie/FS/HFS/Rename.cc
	----------------------
*/

#include "Genie/FS/HFS/Rename.hh"

// Standard C++
#include <algorithm>

// MoreFiles
#include "MoreFiles/MoreFilesExtras.h"

// Debug
#include "debug/assert.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Nitrogen/Files.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// MacIO
#include "MacIO/GetCatInfo_Sync.hh"

// Genie
#include "Genie/MacFilenameFromUnixFilename.hh"
#include "Genie/FS/FSSpec.hh"
#include "Genie/FS/HFS/LongName.hh"
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	namespace K = Kerosene;
	
	
	static bool NamesAreSame( ConstStr63Param a, ConstStr63Param b )
	{
		// b may be a null string to indicate a will be reused, so they're the same
		return b[0] == 0  ||  std::equal( a,
		                                  a + 1 + a[0],
		                                  b );
	}
	
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
	
	static void RenameItem( const FSSpec& srcFile, const FSSpec& destFile )
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
	
	
	void Rename_HFS( const FSSpec& srcFileSpec, const FSTreePtr& destFile )
	{
		if ( !io::item_exists( srcFileSpec ) )
		{
			p7::throw_errno( ENOENT );
		}
		
		bool destExists = destFile->Exists();
		
		bool srcIsDir  = io::directory_exists( srcFileSpec );
		bool destIsDir = destFile->IsDirectory();
		
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
		
		const plus::string& destName = destFile->Name();
		
		const bool keeping_name =    destName.length() == srcFileSpec.name[0]
		                          && std::equal( destName.begin(),
		                                         destName.end(),
		                                         (const char*) srcFileSpec.name + 1 );
		
		if ( srcFileSpec.parID == destFileSpec.parID )
		{
			// Same dir.
			
			if ( keeping_name )
			{
				// Identical names in the same directory.  Nothing to do.
				return;
			}
			
			if ( destExists )
			{
				const bool same_file = NamesAreSame( srcFileSpec.name, destFileSpec.name );
				
				if ( !same_file )
				{
					// Delete existing dest file
					OSErr err = ::FSpDelete( &destFileSpec );
					
					if ( destIsDir  &&  err == fBsyErr )
					{
						p7::throw_errno( ENOTEMPTY );
					}
					
					N::ThrowOSStatus( err );
				}
				
				// Overwrite actual name with requested name
				
				plus::string requested_name = K::MacFilenameFromUnixFilename( destName );
				
				N::CopyToPascalString( requested_name, destFileSpec.name, 31 );
			}
			
			RenameItem( srcFileSpec, destFileSpec );
			
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

