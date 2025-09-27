/*
	buffers.cc
	----------
*/

#include "buffers.hh"

// ams-common
#include "interrupts.hh"


#pragma exceptions off


audio_buffer* free_audio_buffer;

audio_buffer* alloc_buffer()
{
	short saved_SR = disable_interrupts();
	
	audio_buffer* buffer = free_audio_buffer;
	
	if ( buffer )
	{
		free_audio_buffer = buffer->next;
	}
	
	reenable_interrupts( saved_SR );
	
	return buffer;
}

void return_buffer( audio_buffer* buffer )
{
	short saved_SR = disable_interrupts();
	
	buffer->next = free_audio_buffer;
	
	free_audio_buffer = buffer;
	
	reenable_interrupts( saved_SR );
}
