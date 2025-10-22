/*
	lock_high_bit.hh
	----------------
*/

#ifndef LOCKHIGHBIT_HH
#define LOCKHIGHBIT_HH


typedef unsigned char Boolean;
typedef unsigned char Byte;


#ifdef __MC68K__

inline
asm
Boolean lock_high_bit( void* lock : __A0 )
{
	TAS.B    (A0)
	SPL.B    D0
}

#else

inline
Boolean lock_high_bit( void* lock )
{
	/*
		This isn't atomic in the general case, but it should be
		good enough for our scenario of code running at SystemTask
		time that might be preempted by code from interrupt time.
	*/
	
	volatile Byte* p = (Byte*) lock;
	
	/*
		If we're preempted here, the high byte reads as locked.
	*/
	
	Byte high_byte = *p;
	
	/*
		If we're preempted here, the high byte reads as unlocked,
		but the preempting code will take the lock, setting both
		high bits (atomically, from our perspective), so the low
		byte will read as locked below.
	*/
	
	*p++ = high_byte | 0x80;
	
	if ( high_byte & 0x80 )
	{
		return false;  // already locked
	}
	
	/*
		If we're not preempted prior to this point, the preempting
		code will see the high byte as locked and give up.
	*/
	
	Byte low_byte = *p;
	
	*p = low_byte | 0x80;
	
	if ( low_byte & 0x80 )
	{
		return false;  // already locked
	}
	
	return true;  // lock acquired
}

#endif

#endif
