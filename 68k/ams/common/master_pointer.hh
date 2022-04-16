/*
	master_pointer.hh
	-----------------
*/

#ifndef MASTERPOINTER_HH
#define MASTERPOINTER_HH

struct master_pointer
{
	typedef unsigned char   UInt8;
	typedef unsigned short  UInt16;
	typedef unsigned long   UInt32;
	
	char*   alloc;
	UInt8   flags;
	UInt8   reserved;
	UInt16  offset;
	UInt32  type;
	void*   base;
};

#endif
