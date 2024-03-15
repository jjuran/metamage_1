/*
	patches.hh
	----------
*/

#ifndef PATCHES_HH
#define PATCHES_HH

/*
	We don't patch _StillDown ourselves, so we don't need to call a
	prior handler.  We only need to know if _StillDown subsequently
	gets patched by the application, so we can make a point of calling
	it (via A-trap) if so.  Since we'll just be comparing addresses,
	the actual pointer type doesn't matter and `void*` is sufficient.
*/

void* toolbox_trap_table[] : 0x0C00;

extern void* old_StillDown;

static inline
void*& StillDown_handler()
{
	enum { _StillDown = 0xA973 };
	
	return toolbox_trap_table[ _StillDown & 0x03FF ];
}

static inline
bool is_StillDown_patched()
{
	return StillDown_handler() != old_StillDown;
}

#endif
