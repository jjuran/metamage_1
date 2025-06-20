/*
	synthesize.cc
	-------------
*/

#include "synthesize.hh"

// Standard C
#include <stddef.h>
#include <stdint.h>
#include <string.h>

// libsndtrack
#include "admin.hh"
#include "exceptions.hh"
#include "output.hh"
#include "sound_node.hh"
#include "synch.hh"
#include "synth/four-tone.hh"
#include "synth/free-form.hh"
#include "synth/square-wave.hh"


#pragma exceptions off


using sndpipe::set_loudness_level;
using sndpipe::ftMode_flat_buffer;
using sndpipe::ftMode_flat_update;
using sndpipe::ftMode_wave_update;


static inline
void update_fourtone( sound_node* dst, const sound_node* src )
{
	using sndpipe::FTSynthRec_flat_header;
	
	const int size = sizeof (FTSynthRec_flat_header)
	               - offsetof( FTSynthRec_flat_header, duration );
	
	memcpy( &dst->sound.four_tone.duration,
	        &src->sound.four_tone.duration,
	        size );
}

static inline
void update_fourtone_wave( sound_node* dst, const sound_node* src )
{
	int channel = src->sound.wave_data.channel;
	
	uint8_t* wave = dst->sound.four_tone.sound1Wave + channel * sizeof (Wave);
	
	memcpy( wave, src->sound.wave_data.waveform, sizeof (Wave) );
}

static
bool matching_fourtone( const queue_node* node, const void* data )
{
	const sound_node* sound = (const sound_node*) node;
	const sound_node* admin = (const sound_node*) data;
	
	if ( node->size <= 0  ||  sound->sound.mode != ftMode_flat_buffer )
	{
		return false;
	}
	
	using sndpipe::FTSynthRec_flat_header;
	
	const FTSynthRec_flat_header& buffer = sound->sound.four_tone;
	const FTSynthRec_flat_header& update = admin->sound.four_tone;
	
	return buffer.recID == update.recID;
}

static
bool is_sentinel( const queue_node* node, const void* data )
{
	const sound_node* sound = (const sound_node*) node;
	
	return node->size < 0  &&  sound->sound.mode == full_stop_mode;
}

static
sound_node* find_fourtone( const sound_node* update )
{
	return (sound_node*) find( sound_queue, &matching_fourtone, update );
}

static inline
sound_node* get_next_sound()
{
	queue_node* node = queue_get_next( sound_queue );
	
	return node->valid() ? (sound_node*) node : NULL;
}

static
void diminish( sample_buffer& output )
{
	output_sample_t* data = output.data;
	
	if ( output.count > 255 )
	{
		for ( short i = 1;  i < 256;  ++i )
		{
			data[ i ] = data[ i ] * (256 - i) / 256;
		}
		
		output.count = 256;
	}
}

static short audio_level = 7;

static
void soften( sample_buffer& output )
{
	output_sample_t* data = output.data;
	
	for ( short i = 0;  i < 370;  ++i )
	{
		data[ i ] = data[ i ] * audio_level / 7;
	}
}

static bool sounding_2ago;
static bool sounding_then;
static bool sounding_now;

static
short synthesize( sample_buffer& output )
{
	static sound_node* last_input;
	
	sounding_2ago = sounding_then;
	sounding_then = sounding_now;
	sounding_now  = false;
	
	output.count = samples_per_buffer;  // optimistic default
	
	bool stopping = false;
	
	while ( queue_node* admin = queue_get_next( admin_queue ) )
	{
		if ( ! admin->valid() )
		{
			break;
		}
		
		if ( admin->size > 0 )
		{
			sound_node* update = (sound_node*) admin;
			
			if ( update->sound.mode == set_loudness_level )
			{
				using sndpipe::volume_setting;
				
				volume_setting* setting = (volume_setting*) &update->sound;
				
				audio_level = setting->volume & 7;
			}
			
			if ( update->sound.mode == ftMode_flat_update )
			{
				if ( sound_node* node = find_fourtone( update ) )
				{
					update_fourtone( node, update );
				}
			}
			
			if ( update->sound.mode == ftMode_wave_update )
			{
				if ( sound_node* node = find_fourtone( update ) )
				{
					update_fourtone_wave( node, update );
				}
			}
		}
		
		if ( admin->size == full_stop_size  &&  last_input != NULL )
		{
			stopping = true;
			break;
		}
		
		switch ( admin->size )
		{
			case full_stop_size:
				if ( ! void_through( sound_queue, &is_sentinel, NULL ) )
				{
					/*
						We got to the command node in the admin queue before
						the other thread could enqueue the sentinel here.
						Emit silence now and try again later.
					*/
					
					return 0;
				}
				
				// If a sound command followed the sentinel, go process it
				break;
			
			default:
				break;
		}
		
		queue_advance( admin_queue );  // discard the command
	}
	
	while ( sound_node* input = get_next_sound() )
	{
		const bool reset = input != last_input;
		
		last_input = input;
		
		if ( reset  &&  stopping )
		{
			last_input = NULL;
			return 0;
		}
		
		short count = -1;
		
		sound_buffer& sound = input->sound;
		
		switch ( sound.mode )
		{
			case shutdown_mode:
				synch::finished();
				break;
			
			case swMode:
				count = sw_synth( output, sound.square_wave, reset );
				break;
			
			case ffMode:
				count = ff_synth( output, input->size, sound.free_form, reset );
				
				/*
					For audio frame gap detection, we're only interested
					in low-level audio produced by writing to the sound
					buffer, which we'll heuristically characterize as any
					freeform sound whose length is 370 bytes.
				*/
				
				sounding_now = count > 0  &&  input->size == 370;
				break;
			
			case ftMode_flat_buffer:
				count = ft_synth( output, sound.four_tone, reset );
				
				if ( stopping )
				{
					ft_clear();
				}
				
				break;
			
			default:
				break;
		}
		
		if ( count >= 0 )
		{
			if ( audio_level == 0 )
			{
				return 0;
			}
			
			if ( audio_level != 7 )
			{
				soften( output );
			}
			
			if ( sounding_now  &&  ! sounding_then  &&  sounding_2ago )
			{
				take_exception( audio_playback_gap );
			}
			
			if ( stopping  ||  count == 0 )
			{
				last_input = NULL;
				
				diminish( output );
			}
			
			return output.count;
		}
		
		queue_advance( sound_queue );
	}
	
	return 0;
}

const int ground = 0;  // signed samples are centered at 0, not e.g. 128

short synthesize( void* output )
{
	sample_buffer buffer;
	
	short count = synthesize( buffer );
	
	if ( count )
	{
		memcpy( output, buffer.data, count * frame_size );
	}
	
	if ( short n_unset = frames_per_buffer - count )
	{
		n_unset *= frame_size;
		
		char* gap = (char*) output + count * frame_size;
		
		memset( gap, ground, n_unset );  // fill gap with silence
	}
	
	return count;
}
