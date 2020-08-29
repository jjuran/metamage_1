/*
	four-tone.hh
	------------
*/

#ifndef SYNTH_FOURTONE_HH
#define SYNTH_FOURTONE_HH

struct sample_buffer;

namespace sndpipe
{
	struct FTSynthRec_flat_buffer;
}

typedef sndpipe::FTSynthRec_flat_buffer ft_buffer;

short ft_synth( sample_buffer& output, ft_buffer& rec, bool reset );

#endif
