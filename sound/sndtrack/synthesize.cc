/*
	synthesize.cc
	-------------
*/

#include "synthesize.hh"

// Standard C
#include <stddef.h>
#include <string.h>

// sndtrack
#include "admin.hh"
#include "backend.hh"
#include "output.hh"
#include "sound_node.hh"
#include "synth/four-tone.hh"
#include "synth/free-form.hh"
#include "synth/square-wave.hh"


using exosnd::ftMode_flat_buffer;
using exosnd::ftMode_flat_update;


static inline
void update_fourtone( sound_node* dst, const sound_node* src )
{
	using exosnd::FTSynthRec_flat_header;
	
	const int size = sizeof (FTSynthRec_flat_header)
	               - offsetof( FTSynthRec_flat_header, duration );
	
	memcpy( &dst->sound.four_tone.duration,
	        &src->sound.four_tone.duration,
	        size );
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
	
	using exosnd::FTSynthRec_flat_header;
	
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
void diminish( sample_buffer& output, short count )
{
	uint8_t* data = output.data;
	
	if ( count > 255 )
	{
		for ( short i = 1;  i < 256;  ++i )
		{
			int8_t sample = data[ i ] ^ 0x80;
			
			sample = sample * (256 - i) / 256u;
			
			data[ i ] = sample ^ 0x80;
		}
		
		memset( data + 256, 0x80, count - 256 );
	}
}

short synthesize( sample_buffer& output )
{
	static sound_node* last_input;
	
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
			
			if ( update->sound.mode == ftMode_flat_update )
			{
				if ( sound_node* node = find_fourtone( update ) )
				{
					update_fourtone( node, update );
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
				if ( ! void_until( sound_queue, &is_sentinel, NULL ) )
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
				backend::finished();
				break;
			
			case swMode:
				count = sw_synth( output, sound.square_wave, reset );
				break;
			
			case ffMode:
				count = ff_synth( output, input->size, sound.free_form, reset );
				break;
			
			case ftMode_flat_buffer:
				count = ft_synth( output, sound.four_tone, reset );
				break;
			
			default:
				break;
		}
		
		if ( count >= 0 )
		{
			if ( stopping )
			{
				last_input = NULL;
				
				diminish( output, count );
			}
			
			return count;
		}
		
		queue_advance( sound_queue );
	}
	
	return 0;
}
