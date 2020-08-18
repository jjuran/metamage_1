/*
	sndtrack.cc
	-----------
*/

// POSIX
#include <unistd.h>

// Standard C
#include <signal.h>

// iota
#include "iota/endian.hh"

// exosnd-api
#include "exosnd/endian.hh"

// sndtrack
#include "admin.hh"
#include "backend.hh"
#include "exceptions.hh"
#include "input.hh"
#include "sound_node.hh"


using namespace exosnd;

struct command_header
{
	uint16_t reserved;
	uint16_t length;
	uint16_t domain;
	uint16_t opcode;
};

static
void byte_swap( command_header& header )
{
	header.length = iota::swap_2_bytes( header.length );
	header.domain = iota::swap_2_bytes( header.domain );
	header.opcode = iota::swap_2_bytes( header.opcode );
}

static
void check_tones( const Tone* tone, short n )
{
	while ( n-- > 0 )
	{
		if ( tone->count == 0 )
		{
			if ( tone->amplitude != 0  ||  tone->duration != 0 )
			{
				take_exception( element_wavelength_zero );
			}
			else if ( n != 0 )
			{
				take_exception( opcode_length_overlong );
			}
			
			return;
		}
		
		++tone;
	}
	
	take_exception( sound_unterminated );
}

static
void check_squarewave( const command_header& header, const SWSynthRec& rec )
{
	if ( header.length % sizeof (Tone) != 0 )
	{
		take_exception( opcode_length_invalid );
	}
	
	check_tones( rec.triplets, header.length / sizeof (Tone) );
}

static
ssize_t read_header( int fd, command_header& header )
{
	ssize_t n_read = read_all( fd, &header, sizeof header );
	
	if ( n_read > 0 )
	{
		exosnd::endianize( header );
	}
	else if ( n_read == 0 )
	{
		take_exception( header_read_zero );
		
		// exit from the event loop normally
	}
	else if ( n_read == -1 )
	{
		take_exception( stream_read_error );
	}
	else
	{
		take_exception( header_read_short );
	}
	
	return n_read;
}

static
void read_and_enqueue( int fd, const command_header& header, rt_queue& queue )
{
	const unsigned node_size = sizeof (sound_node)
	                         - sizeof (sound_buffer)
	                         + sizeof header.opcode
	                         + header.length;
	
	if ( sound_node* node = (sound_node*) queue_alloc_node( node_size ) )
	{
		const short mode = header.opcode;
		
		short* addr = &node->sound.mode;
		
		*addr++ = mode;
		
		ssize_t n_read = read_all( fd, addr, header.length );
		
		if ( n_read <= 0 )
		{
			take_exception( extent_read_short );
		}
		
		node->size = 1;  // Mostly arbitrary value indicating validity
		
		switch ( mode )
		{
			case swMode:
				check_squarewave( header, node->sound.square_wave );
				
				endianize( node->sound.square_wave );
				break;
			
			case ffMode:
				if ( header.length < sizeof (Fixed) )
				{
					take_exception( opcode_length_underrun );
				}
				
				node->size = header.length - sizeof (Fixed);
				
				if ( node->size == 0 )
				{
					take_exception( sound_empty );
				}
				
				endianize( node->sound.free_form );
				break;
			
			case ftMode_flat_buffer:
			case ftMode_flat_update:
				endianize( node->sound.four_tone );
				break;
			
			default:
				take_exception( domain_opcode_undefined );
				break;
		}
		
		queue_append( queue, node );
	}
}

static
void do_full_stop()
{
	if ( queue_node* node = queue_alloc_node( sizeof (queue_node) ) )
	{
		node->size = full_stop_size;
		
		queue_append( admin_queue, node );
	}
	
	const int dummy_size = sizeof (sound_node)
	                     - sizeof (sound_buffer)
	                     + sizeof (short);
	
	if ( sound_node* node = (sound_node*) queue_alloc_node( dummy_size ) )
	{
		node->size       = full_stop_size;
		node->sound.mode = full_stop_mode;
		
		queue_append( sound_queue, node );
	}
}

static
void do_shutdown()
{
	const int dummy_size = sizeof (sound_node)
	                     - sizeof (sound_buffer)
	                     + sizeof (short);
	
	if ( sound_node* node = (sound_node*) queue_alloc_node( dummy_size ) )
	{
		node->size       = shutdown_size;
		node->sound.mode = shutdown_mode;
		
		queue_append( sound_queue, node );
	}
}

static sig_atomic_t interrupted;

static
void event_loop( int fd )
{
	bool audio_started = false;
	
	command_header header;
	
	while ( ! interrupted  &&  read_header( fd, header ) > 0 )
	{
		if ( header.reserved != 0 )
		{
			take_exception( header_reserved_field_violation );
		}
		
		switch ( header.domain )
		{
			case basic_domain:
				switch ( header.opcode )
				{
					case switch_on:
						static bool init = audio_started = backend::start();
						break;
					
					case full_stop:
						do_full_stop();
						break;
					
					default:
						take_exception( domain_opcode_undefined );
						break;
				}
				break;
			
			case admin_domain:
				read_and_enqueue( fd, header, admin_queue );
				break;
			
			case sound_domain:
				read_and_enqueue( fd, header, sound_queue );
				break;
			
			default:
				take_exception( header_domain_undefined );
				break;
		}
		
		queue_cull_used( admin_queue );
		queue_cull_used( sound_queue );
	}
	
	if ( audio_started )
	{
		if ( interrupted )
		{
			do_full_stop();
			
			backend::wait();  // consume the interrupt
		}
		
		do_shutdown();
		
		if ( backend::wait() )
		{
			do_full_stop();
			do_shutdown();
		}
		
		backend::stop();
	}
}

static
void interrupt( int )
{
	interrupted = true;
	
	backend::interrupted();
}

int main( int argc, char* argv[] )
{
	signal( SIGINT, &interrupt );
	
	event_loop( STDIN_FILENO );
	
	return 0;
}
