/*
	listing.cc
	----------
*/

#include "mac_file/listing.hh"

// mac-sys-utils
#include "mac_sys/mem_error.hh"

// mac-file-utils
#include "mac_file/FSGetCatalogInfo.hh"


#pragma exceptions off


namespace mac  {
namespace file {

#if ! __LP64__

static inline
OSErr SetHandleSize( Handle h, Size size )
{
	::SetHandleSize( h, size );
	
	return mac::sys::mem_error();
}

static inline
OSErr get_name_from_iterator( FSIterator it, Byte* hfs_name )
{
	OSErr err;
	FSSpec spec;
	ItemCount count;
	HFSUniStr255 name;
	
	err = FSGetCatalogInfoBulk( it,
	                            1,
	                            &count,
	                            NULL,
	                            0,
	                            NULL,
	                            NULL,
	                            &spec,
	                            &name );
	
	/*
		We aren't trusting the length byte in spec.name,
		so this is safe whether the call succeeded or not.
	*/
	
	BlockMoveData( spec.name, hfs_name, sizeof (Str63) );
	
	if ( err == noErr  &&  spec.name[ 0 ] > name.length )
	{
		err = Str63_from_HFSUniStr255( hfs_name, name );
	}
	
	return err;
}

OSErr list_directory( Handle           result,
                      short            vRefNum,
                      long             dirID,
                      ConstStr63Param  name )
{
	OSErr err;
	CInfoPBRec parent;
	
	parent.dirInfo.ioVRefNum = vRefNum;
	parent.dirInfo.ioDrDirID = dirID;
	parent.dirInfo.ioNamePtr = (StringPtr) name;
	
	parent.dirInfo.ioFDirIndex = name ? 0 : -1;
	
	err = PBGetCatInfoSync( &parent );
	
	if ( err )
	{
		// An error occurred -- bail out.
	}
	else if ( parent.dirInfo.ioFlAttrib & kioFlAttribDirMask )
	{
		// It's a directory -- scan it.
		
		dirID = parent.dirInfo.ioDrDirID;
		
		const UInt32 max_n = (UInt32) -1 / sizeof (list_entry);
		
		const UInt32 n = parent.dirInfo.ioDrNmFls;
		
		(n > max_n  &&  (err = memFullErr))  ||
		(err = SetHandleSize( result, n * sizeof (list_entry) ));
		
		// `result` is a valid handle even if SetHandleSize() failed.
		
		list_entry* next = (list_entry*) *result;
		
		FSIterator it;
		
		if ( TARGET_API_MAC_CARBON  &&  err == noErr )
		{
			FSRef ref;
			FSRefParam pb;
			
			pb.ioNamePtr = name;
			pb.ioVRefNum = vRefNum;
			pb.ioDirID   = dirID;
			pb.newRef    = &ref;
			
			err                             ||
			(err = PBMakeFSRefSync( &pb ))  ||
			(err = FSOpenIterator( &ref, kFSIterateFlat, &it ));
			
			if ( err )
			{
				return err;
			}
		}
		
		for ( int i = 1;  ! err  &&  i <= (int) n;  ++i, ++next )
		{
			CInfoPBRec& child = next->cInfo;
			
			if ( TARGET_API_MAC_CARBON )
			{
				err = get_name_from_iterator( it, next->name );
				
				if ( err )
				{
					break;
				}
				
				child.dirInfo.ioFDirIndex = 0;
			}
			else
			{
				next->name[ 0 ] = 0;
				
				child.dirInfo.ioFDirIndex = i;  // one-based indexing
			}
			
			child.dirInfo.ioVRefNum = vRefNum;
			child.dirInfo.ioDrDirID = dirID;
			child.dirInfo.ioNamePtr = next->name;
			
			err = PBGetCatInfoSync( &child );
			
			/*
				We're storing the filenames and CInfo records in a
				handle, which might subsequently be moved in memory,
				invalidating any pointers dereferencing it.  To be
				safe, we'll preemptively clear ioNamePtr so as to
				prevent accidental use of it after potential memory
				movement that would be challenging even to reproduce
				(as well as fiendishly difficult to debug).
			*/
			
			child.dirInfo.ioNamePtr = NULL;
		}
		
		if ( TARGET_API_MAC_CARBON )
		{
			FSCloseIterator( it );
		}
	}
	else
	{
		// It exists, but it's not a directory.  Report it.
		
		err = dirNFErr;
	}
	
	return err;
}

#else

int dummy;

#endif  // #if ! __LP64__

}
}
