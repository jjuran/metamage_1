/*
	Genie/FS/HFS/MoveRename.cc
	--------------------------
*/

#include "Genie/FS/HFS/MoveRename.hh"

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// Standard C
#include <time.h>

// mac-sys-utils
#include "mac_sys/volume_params.hh"

// gear
#include "gear/hexadecimal.hh"


namespace Genie
{
	
	static
	bool volume_has_MoveRename( short vRefNum )
	{
		GetVolParmsInfoBuffer info;
		
		OSErr err = mac::sys::get_volume_params( info, vRefNum );
		
		return err == noErr  &&  info.vMAttrib & (1 << bHasMoveRename);
	}
	
	static
	void fill_temp_name( Str31 name )
	{
		/*
			The clock is in microseconds.  It may have only tick resolution
			in System 6, but it should meet our needs anyway.
		*/
		
		clock_t us = clock();
		
		char* p = (char*) name;
		
		*p++ = 2 + sizeof us * 2;  // 18 bytes
		*p++ = '(';
		
		p = gear::hexpcpy_lower( p, &us, sizeof us );
		
		*p++ = ')';
	}
	
	static
	OSErr MoveRename( short                 vRefNum,
	                  SInt32                src_parID,
	                  const unsigned char*  src_name,
	                  SInt32                dst_dirID,
	                  const unsigned char*  new_name )
	{
		if ( volume_has_MoveRename( vRefNum ) )
		{
			HParamBlockRec pb;
			
			pb.copyParam.ioVRefNum  = vRefNum;
			pb.copyParam.ioDirID    = src_parID;
			pb.copyParam.ioNamePtr  = (StringPtr) src_name;
			pb.copyParam.ioNewDirID = dst_dirID;
			pb.copyParam.ioNewName  = NULL;
			pb.copyParam.ioCopyName = (StringPtr) new_name;
			
			return ::PBHMoveRenameSync( &pb );
		}
		
		if ( new_name == NULL )
		{
			// No need to rename, just move
			
			return CatMove( vRefNum, src_parID, src_name, dst_dirID, NULL );
		}
		
		OSErr err, err2;
		
		// Try to rename in place
		
		err = HRename( vRefNum, src_parID, src_name, new_name );
		
		if ( err == noErr )
		{
			// Okay, now move
			
			err = CatMove( vRefNum, src_parID, new_name, dst_dirID, NULL );
			
			if ( err != noErr )
			{
				// Move failed, roll back the rename.  Hopefully this works...
				
				err2 = HRename( vRefNum, src_parID, new_name, src_name );
			}
			
			return err;
		}
		
		if ( err != dupFNErr )
		{
			// Locked volume, maybe
			return err;
		}
		
		// Duplicate filename in source directory.  Try move then rename.
		
		err = CatMove( vRefNum, src_parID, src_name, dst_dirID, NULL );
		
		if ( err == noErr )
		{
			// Okay, now rename
			
			err = HRename( vRefNum, dst_dirID, src_name, new_name );
			
			if ( err != noErr )
			{
				// Rename failed, roll back the move.  Hopefully this works...
				
				err2 = CatMove( vRefNum, dst_dirID, src_name, src_parID, NULL );
			}
			
			return err;
		}
		
		// Try rename/move/rename.
		
		Str31 tmp_name;
		
		fill_temp_name( tmp_name );
		
		err = HRename( vRefNum, src_parID, src_name, tmp_name );
		
		if ( err != noErr )
		{
			return err;  // Just give up
		}
		
		err = CatMove( vRefNum, src_parID, tmp_name, dst_dirID, NULL );
		
		if ( err == noErr )
		{
			err = HRename( vRefNum, dst_dirID, tmp_name, new_name );
			
			if ( err == noErr )
			{
				return err;
			}
			
			err2 = CatMove( vRefNum, dst_dirID, tmp_name, src_parID, NULL );
		}
		
		err2 = HRename( vRefNum, src_parID, tmp_name, src_name );
		
		return err;
	}
	
	OSErr MoveRename( const FSSpec&         src,
	                  const long            dst_dir,
	                  const unsigned char*  new_name )
	{
		return MoveRename( src.vRefNum,
		                   src.parID,
		                   src.name,
		                   dst_dir,
		                   new_name );
	}
	
}
