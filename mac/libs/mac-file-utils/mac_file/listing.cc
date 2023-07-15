/*
	listing.cc
	----------
*/

#include "mac_file/listing.hh"

// mac-sys-utils
#include "mac_sys/mem_error.hh"


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
		
		for ( int i = 1;  ! err  &&  i <= (int) n;  ++i, ++next )
		{
			next->name[ 0 ] = 0;
			
			CInfoPBRec& child = next->cInfo;
			
			child.dirInfo.ioVRefNum = vRefNum;
			child.dirInfo.ioDrDirID = dirID;
			child.dirInfo.ioNamePtr = next->name;
			
			child.dirInfo.ioFDirIndex = i;  // one-based indexing
			
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
