/*
	scoped_open_resfile.hh
	----------------------
*/

#ifndef MACRSRC_SCOPEDOPENRESFILE_HH
#define MACRSRC_SCOPEDOPENRESFILE_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// iota
#include "iota/class.hh"

// mac-rsrc-utils
#include "mac_rsrc/ResFileRefNum.hh"


namespace mac  {
namespace rsrc {

	class scoped_open_resfile
	{
		NON_COPYABLE( scoped_open_resfile )
		NO_NEW_DELETE
		
		private:
			ResFileRefNum its_refnum;
		
		public:
			scoped_open_resfile( ResFileRefNum refnum ) : its_refnum( refnum )
			{
			}
			
			~scoped_open_resfile();
			
			ResFileRefNum get() const  { return its_refnum; }
			
			operator ResFileRefNum() const  { return get(); }
	};
	
	inline
	scoped_open_resfile::~scoped_open_resfile()
	{
		if ( its_refnum > 0 )
		{
			CloseResFile( its_refnum );
		}
	}
	
}
}

#endif
