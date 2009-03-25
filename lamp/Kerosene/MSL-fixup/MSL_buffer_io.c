/*	===============
 *	MSL_buffer_io.c
 *	===============
 */

#ifdef __MWERKS__

#include "buffer_io.h"

// In Genie we use the One True Newline Convention, LF.
// CodeWarrior defaults to 'MPW newlines', in which the mapping is swapped --
// '\n' is CR and '\r' is LF.  We opt out of this, but MSL checks for this and
// swaps them at runtime anyway.
// Here we define the functions ourselves to prevent this.

void __convert_from_newlines( unsigned char*, size_t* )
{
}

void __convert_to_newlines( unsigned char*, size_t* )
{
}

#endif

