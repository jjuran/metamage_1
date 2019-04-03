/*
	four-tone.hh
	------------
*/

#ifndef SYNTH_FOURTONE_HH
#define SYNTH_FOURTONE_HH

struct sample_buffer;

namespace exosnd
{
	struct FTSynthRec_flat_buffer;
}

typedef exosnd::FTSynthRec_flat_buffer ft_buffer;

short ft_synth( sample_buffer& output, ft_buffer& rec, bool reset );

#endif
