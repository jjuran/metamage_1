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

// sndpipe-api
#include "sndpipe/endian.hh"

// command
#include "command/get_option.hh"

// sndtrack
#include "admin.hh"
#include "backend.hh"
#include "exceptions.hh"
#include "input.hh"
#include "portaudio.hh"
#include "recording.hh"
#include "sound_node.hh"


using namespace sndpipe;

enum
{
	Option_record = 'o',
};

static command::option options[] =
{
	{ "rec",    Option_record, command::Param_required },
	{ "record", Option_record, command::Param_required },
	
	{ NULL }
};

static
char* const* get_options( char** argv )
{
	int opt;
	
	++argv;  // skip arg 0
	
	while ( (opt = command::get_option( (char* const**) &argv, options )) > 0 )
	{
		using command::global_result;
		
		switch ( opt )
		{
			case Option_record:
				start_recording( global_result.param );
				break;
			
			default:
				break;
		}
	}
	
	return argv;
}

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
void check_tones( Tone* tone, short n )
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
	
	tone->count = 0;
}

static
void check_squarewave( const command_header& header, SWSynthRec& rec )
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
		sndpipe::endianize( header );
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
			
			case ftMode_wave_update:
				// No endianization needed.
				break;
			
			case set_loudness_level:
				if ( header.length < 2 )
				{
					take_exception( opcode_length_underrun );
				}
				break;
			
			default:
				take_exception( domain_opcode_undefined );
				break;
		}
		
		queue_append( queue, node );
	}
}

static
void ping_pong( int fd, const command_header& header )
{
	/*
		These fields should in theory be byte-swapped to big-endian,
		but each field is a pair of identical bytes, so it's a no-op.
	*/
	
	command_header pong =
	{
		0,  // reserved
		0,  // length
		basic_domain,  // $0101
		pong_sent,     // '<<'
	};
	
	ssize_t n_written = write( fd, &pong, sizeof pong );
	
	if ( n_written < 0 )
	{
		take_exception( socket_write_error );
	}
	else if ( n_written < sizeof pong )
	{
		take_exception( socket_write_short );
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

static
bool start()
{
	return backend::start()  &&  portaudio::start();
}

static
bool stop()
{
	backend::stop();
	
	return portaudio::stop();
}

static sig_atomic_t interrupted;

static
void event_loop( int fd )
{
	bool audio_started = false;
	bool play_past_EOF = false;
	
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
						static bool init = audio_started = start();
						break;
					
					case allow_eof:
						play_past_EOF = true;
						break;
					
					case ping_sent:
						ping_pong( fd, header );
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
		else if ( ! play_past_EOF )
		{
			do_full_stop();
		}
		
		do_shutdown();
		
		if ( backend::wait() )
		{
			do_full_stop();
			do_shutdown();
		}
		
		stop();
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
	if ( argc == 0 )
	{
		return 0;  // whatever
	}
	
	char* const* args = get_options( argv );
	
	int argn = argc - (args - argv);
	
	if ( signal( SIGINT, SIG_IGN ) != SIG_IGN )
	{
		signal( SIGINT, &interrupt );
	}
	
	event_loop( STDIN_FILENO );
	
	end_recording();
	
	return 0;
}
