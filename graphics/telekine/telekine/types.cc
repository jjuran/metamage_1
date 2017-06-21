/*
	types.cc
	--------
*/

#include "telekine/types.hh"

// iota
#include "iota/endian.hh"


#define SWAP2( field )  ((field) = iota::swap_2_bytes( field ))
#define SWAP4( field )  ((field) = iota::swap_4_bytes( field ))


namespace telekine
{
	
	void swap_bytes( command_header& header )
	{
		SWAP4( header.size );
	}
	
	void swap_bytes( raster_desc& desc )
	{
		SWAP4( desc.width  );
		SWAP4( desc.height );
		SWAP4( desc.stride );
	}
	
	void swap_bytes( rect_pv16& rect )
	{
		SWAP2( rect.left   );
		SWAP2( rect.top    );
		SWAP2( rect.width  );
		SWAP2( rect.height );
	}
	
}
