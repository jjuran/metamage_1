/*
	endian.cc
	---------
*/

#include "sndpipe/endian.hh"


namespace sndpipe
{

using namespace iota;

// The caller is assumed to have already swapped the mode field in all cases.

void byte_swap( SWSynthRec& rec )
{
	/*
		A zero count is illegal under the protocol (except as the terminator).
		We assume this has already been checked and reported, but if not, at
		least we don't propagate one here.
	*/
	
	for ( Tone* tone = rec.triplets;  tone->count != 0;  ++tone )
	{
		tone->count     = swap_2_bytes( tone->count     );
		tone->amplitude = swap_2_bytes( tone->amplitude );
		tone->duration  = swap_2_bytes( tone->duration  );
	}
}

void byte_swap( FFSynthRec& rec )
{
	rec.count = swap_4_bytes( rec.count );
}

void byte_swap( FTSynthRec_flat_header& rec )
{
	// recID does not get swapped -- it's just a unique ID.
	
	rec.duration    = swap_2_bytes( rec.duration    );
	
	rec.sound1Rate  = swap_4_bytes( rec.sound1Rate  );
	rec.sound1Phase = swap_4_bytes( rec.sound1Phase );
	rec.sound2Rate  = swap_4_bytes( rec.sound2Rate  );
	rec.sound2Phase = swap_4_bytes( rec.sound2Phase );
	rec.sound3Rate  = swap_4_bytes( rec.sound3Rate  );
	rec.sound3Phase = swap_4_bytes( rec.sound3Phase );
	rec.sound4Rate  = swap_4_bytes( rec.sound4Rate  );
	rec.sound4Phase = swap_4_bytes( rec.sound4Phase );
}

}  // namespace sndpipe
