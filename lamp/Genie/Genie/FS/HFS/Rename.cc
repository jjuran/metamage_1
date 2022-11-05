/*
	Genie/FS/HFS/Rename.cc
	----------------------
*/

#include "Genie/FS/HFS/Rename.hh"

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
#include "vfs/node.hh"
#include "vfs/functions/file-tests.hh"

// MacVFS
#include "MacVFS/util/FSSpec_from_node.hh"

// Io: MacFiles
#include "MacFiles/Classic.hh"

// MacIO
#include "MacIO/GetCatInfo_Sync.hh"

// Genie
#include "Genie/FS/HFS/LongName.hh"
#include "Genie/FS/HFS/MoveRename.hh"
#include "Genie/Utilities/AsyncIO.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace p7 = poseven;
	
	
	static bool FileIsLocked( const FSSpec& file )
	{
		CInfoPBRec cInfo = {{ 0 }};
		
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
					N::HRstFLock( file );
				}
			}
			
			~FileLockBypass()
			{
				if ( itWasLocked )
				{
					::HSetFLock( itsFileSpec.vRefNum,
					             itsFileSpec.parID,
					             itsFileSpec.name );
				}
			}
			
			void SetFile( const FSSpec& file )
			{
				itsFileSpec = file;
			}
	};
	
	static OSErr ForceRenameItem( const FSSpec& src, const FSSpec& dst )
	{
		ASSERT( src.vRefNum == dst.vRefNum );
		ASSERT( src.parID   == dst.parID   );
		
		FileLockBypass lockBypass( src );
		
		// Rename source to dest
		OSErr err = ::HRename( src.vRefNum, src.parID, src.name, dst.name );
		
		if ( err == dupFNErr )
		{
			err = ::HDelete( dst.vRefNum, dst.parID, dst.name );
			
			if ( err != noErr )
			{
				return err;
			}
			
			err = ::HRename( src.vRefNum, src.parID, src.name, dst.name );
		}
		
		Mac::ThrowOSStatus( err );
		
		lockBypass.SetFile( dst );
		
		return noErr;
	}
	
	
	static inline plus::string slashes_from_colons( const plus::string& unix_name )
	{
		return plus::replaced_string( unix_name, ':', '/' );
	}
	
	
	static inline
	void CatMove( const FSSpec& src, long dstDir )
	{
		OSErr err = ::CatMove( src.vRefNum, src.parID, src.name, dstDir, NULL );
		
		Mac::ThrowOSStatus( err );
	}
	
	static void MoveAndRename( const FSSpec& srcFile, const FSSpec& destFile )
	{
		FileLockBypass lockBypass( srcFile );
		
		Mac::ThrowOSStatus( MoveRename( srcFile, destFile.parID, destFile.name ) );
		
		lockBypass.SetFile( destFile );
	}
	
	
	void Rename_HFS( const FSSpec& srcFileSpec, const vfs::node& destFile )
	{
		CInfoPBRec src_pb;
		
		N::FSpGetCatInfo( srcFileSpec, src_pb, N::FNF_Throws() );
		
		bool destExists = exists( destFile );
		
		bool srcIsDir  = io::item_is_directory( src_pb );
		bool destIsDir = is_directory( destFile );
		
		if ( destExists  &&  srcIsDir != destIsDir )
		{
			p7::throw_errno( destIsDir ? EISDIR : ENOTDIR );
		}
		
		FSSpec destFileSpec = vfs::FSSpec_from_node( destFile );
		
		// Can't move across volumes
		if ( srcFileSpec.vRefNum != destFileSpec.vRefNum )
		{
			p7::throw_errno( EXDEV );
		}
		
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
			
			N::HDelete( destFileSpec );
		}
		
		if ( keeping_name )
		{
			// Same name, different dir; move only.
			CatMove( srcFileSpec, destFileSpec.parID );
		}
		else
		{
			// Darn, we have to move *and* rename.
			MoveAndRename( srcFileSpec, destFileSpec );
			
			SetLongName( destFileSpec, destName );
		}
	}
	
}
