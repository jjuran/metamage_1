/*
	free-form.hh
	------------
*/

#ifndef SYNTH_FREEFORM_HH
#define SYNTH_FREEFORM_HH

struct sample_buffer;

typedef struct FFSynthRec ff_buffer;

short ff_synth( sample_buffer& output, int size, ff_buffer& rec, bool reset );

#endif
