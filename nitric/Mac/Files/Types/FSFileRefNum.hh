/*
	Mac/Files/Types/FSFileRefNum.hh
	-------------------------------
*/

#ifndef MAC_FILES_TYPES_FSFILEREFNUM_HH
#define MAC_FILES_TYPES_FSFILEREFNUM_HH

#ifndef __FILES__
#include <Files.h>
#endif

// nucleus
#include "nucleus/enumeration_traits.hh"
#include "nucleus/disposer.hh"


namespace Mac
{
	
	enum FSFileRefNum
	{
		kFSFileRefNum_Max = nucleus::enumeration_traits< SInt16 >::max
	};
	
}

namespace nucleus
{
	
	template <> struct disposer< Mac::FSFileRefNum >
	{
		typedef Mac::FSFileRefNum  argument_type;
		typedef void               result_type;
		
		void operator()( Mac::FSFileRefNum file ) const
		{
			::FSClose( file );
		}
	};
	
}

#endif

