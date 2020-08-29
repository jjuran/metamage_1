/*
	endian.hh
	---------
*/

#ifndef SNDPIPE_ENDIAN_HH
#define SNDPIPE_ENDIAN_HH

// iota
#include "iota/endian.hh"

// sndpipe
#include "sndpipe/sndpipe.hh"


namespace sndpipe
{
	
	// These **do not** byte-swap the mode field!
	
	void byte_swap( SWSynthRec& rec );
	void byte_swap( FFSynthRec& rec );
	
	void byte_swap( FTSynthRec_flat_header& rec );
	
	template < class T >
	inline
	void endianize( T& rec )
	{
		if ( iota::is_little_endian() )
		{
			byte_swap( rec );
		}
	}
	
}  // namespace sndpipe

#endif
