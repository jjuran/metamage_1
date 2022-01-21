/*
	exceptions.hh
	-------------
*/

#ifndef EXCEPTIONS_HH
#define EXCEPTIONS_HH

enum exception_type
{
	no_error,
	out_of_memory,
	audio_platform_start_error,
	audio_platform_stop_error,
	stream_read_error,
	header_read_zero,
	header_read_short,
	header_reserved_field_violation,  // header.reserved != 0
	header_domain_undefined,
	domain_opcode_undefined,
	opcode_length_underrun,           // e.g. free-form < 4
	opcode_length_overlong,           // e.g. four-tone update > 38
	opcode_length_invalid,            // e.g. not (k * 6) for square waves
	extent_read_short,
	sound_empty,                      // zero tones or free-form wave bytes
	sound_unterminated,               // missing 0,0,0 tone element
	sound_playback_rate_zero,         // free-form wave with count == 0
	sound_duration_out_of_bounds,     // SW tone or FT duration > 255
	sound_duration_zero,              // FT or cumulative SW duration == 0
	element_wavelength_zero,          // square-wave tone with count == 0
	element_duration_zero,            // square-wave tone duration == 0
	update_unidentified,              // four-tone update's ID doesn't match
	audio_playback_gap,               // suspicious gap between sounds
};

/*
	Zero-duration for individual tones and for entire sounds are distinct
	conditions -- one might want to warn on the latter but not the former.
	Whereas an out-of-bounds duration is just as invalid in an individual
	square-wave tone element as in a four-tone sound.  But, the /cumulative/
	duration of a square-wave sound isn't limited at all.
*/

bool take_exception_internal( exception_type type );

void take_exception( exception_type type );

#endif
