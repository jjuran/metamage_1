/*
	note.hh
	-------
*/

#ifndef RASTER_NOTE_HH
#define RASTER_NOTE_HH

// Standard C
#include <stddef.h>
#include <stdint.h>


namespace raster
{
	
	/*
		Notes are additional metadata sections following the description that
		provide supplementary information about a raster image.  Notes are by
		nature optional to include -- since any universally required details
		would have been added to the description -- but they vary in whether
		the *use* of them is mandatory.  For example, an indexed color image
		is likely to be unusable without applying a palette, a non-rectangular
		image is still viewable without applying a mask region, and a segment
		that hosts a process-shared synchronization structure can be ignored
		completely by a still-frame grabber without ill effect (mostly).
	*/
	
	enum note_type
	{
		Note_end = 0,
	};
	
	const size_t note_alignment = 4;
	
	struct raster_note
	{
		uint32_t  type;   // note_type
		uint16_t  size;   // doesn't include the raster_note header
		uint16_t  flags;  // reserved
	};
	
	
	inline
	bool exists( const raster_note* note )
	{
		return note->type != Note_end;
	}
	
	inline
	const void* data( const raster_note* note )
	{
		return note + 1;
	}
	
	inline
	void* data( raster_note* note )
	{
		return note + 1;
	}
	
	template < class Data >
	inline
	const Data& data( const raster_note& note )
	{
		return *static_cast< const Data* >( data( &note ) );
	}
	
	template < class Data >
	inline
	Data& data( raster_note& note )
	{
		return *static_cast< Data* >( data( &note ) );
	}
	
	inline
	const raster_note* next( const raster_note* note )
	{
		const size_t mask = note_alignment - 1;
		
		size_t size = note++->size;
		
		size = size + mask & ~mask;  // pad for alignment
		
		return (const raster_note*) ((const char*) note + size);
	}
	
	inline
	raster_note* next( raster_note* note )
	{
		return const_cast< raster_note* >( next( (const raster_note*) note ) );
	}
	
	inline
	raster_note& next( raster_note& note )
	{
		return *next( &note );
	}
	
	template < class Note >
	inline
	Note* find( Note* note, note_type type )
	{
		const raster_note* typecheck = note;
		
		while ( exists( note ) )
		{
			if ( note->type == type )
			{
				return note;
			}
			
			note = next( note );
		}
		
		return NULL;
	}
	
}

#endif
