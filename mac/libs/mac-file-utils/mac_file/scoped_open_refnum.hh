/*
	scoped_open_refnum.hh
	---------------------
*/

#ifndef MACFILE_SCOPEDOPENREFNUM_HH
#define MACFILE_SCOPEDOPENREFNUM_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __FILES__
#include <Files.h>
#endif

// iota
#include "iota/class.hh"

// mac-file-utils
#include "mac_file/FSIORefNum.hh"


namespace mac  {
namespace file {
	
	class scoped_open_refnum
	{
		NON_COPYABLE( scoped_open_refnum )
		NO_NEW_DELETE
		
		private:
			FSIORefNum its_refnum;
		
		public:
			scoped_open_refnum( FSIORefNum refnum ) : its_refnum( refnum )
			{
			}
			
			~scoped_open_refnum();
			
			FSIORefNum get() const  { return its_refnum; }
			
			operator FSIORefNum() const  { return get(); }
	};
	
#if ! __LP64__
	
	inline
	scoped_open_refnum::~scoped_open_refnum()
	{
		if ( its_refnum > 0 )
		{
			FSClose( its_refnum );
		}
	}
	
#endif  // #if ! __LP64__
	
}
}

#endif
