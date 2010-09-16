/*
	serialize_qd.hh
	---------------
*/

#ifndef GENIE_FS_SERIALIZEQD_HH
#define GENIE_FS_SERIALIZEQD_HH

// plus
#include "plus/deconstruct.hh"
#include "plus/reconstruct.hh"
#include "plus/serialize.hh"

// Genie
#include "Genie/FS/stringify_qd.hh"
#include "Genie/FS/vivify_qd.hh"


namespace Genie
{
	
	struct serialize_Point : plus::serialize_POD< Point >
	{
		typedef stringify_Point  stringify;
		typedef vivify_Point     vivify;
		
		typedef plus::deconstruct< freeze, stringify >    deconstruct;
		typedef plus::reconstruct< Point, thaw, vivify >  reconstruct;
	};
	
	struct serialize_Rect : plus::serialize_POD< Rect >
	{
		typedef stringify_Rect  stringify;
		typedef vivify_Rect     vivify;
		
		typedef plus::deconstruct< freeze, stringify >   deconstruct;
		typedef plus::reconstruct< Rect, thaw, vivify >  reconstruct;
	};
	
}

#endif

