/*
	datum_storage.hh
	----------------
*/

#ifndef VXO_DATUMSTORAGE_HH
#define VXO_DATUMSTORAGE_HH


namespace vxo
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
		datum_buffer_size = sizeof (datum_allocation),
		datum_max_offset  = datum_buffer_size - 1
	};
	
	struct datum_storage
	{
		union
		{
			char              small[ datum_buffer_size ];
			datum_allocation  alloc;
		};
	};
	
	struct datum_movable : datum_storage {};
	
}

#endif
