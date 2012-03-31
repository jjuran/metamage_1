/*
	datum_storage.hh
	----------------
*/

#ifndef PLUS_DATUMSTORAGE_HH
#define PLUS_DATUMSTORAGE_HH


namespace plus
{
	
	struct datum_allocation
	{
		const char*  pointer;
		long         length;
		long         capacity;
		const void*  misc;
	};
	
	enum
	{
		datum_buffer_size = sizeof (datum_allocation)
	};
	
	struct datum_storage
	{
		union
		{
			char              small[ datum_buffer_size ];
			datum_allocation  alloc;
		};
	};
	
}

#endif

