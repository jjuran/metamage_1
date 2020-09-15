/*
	buffers.hh
	----------
*/

#ifndef BUFFERS_HH
#define BUFFERS_HH

void* alloc_buffer( unsigned minimum_length );

void free_buffer( void* buffer );

#endif
