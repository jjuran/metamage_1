/*
	endian.hh
	---------
*/

#ifndef EXOSND_ENDIAN_HH
#define EXOSND_ENDIAN_HH

// iota
#include "iota/endian.hh"

// exosnd
#include "exosnd/exosnd.hh"


namespace exosnd
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
	
}  // namespace exosnd

#endif
