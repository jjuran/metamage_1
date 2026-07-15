/*
	cursor.hh
	---------
*/

#ifndef CURSOR_CURSOR_HH
#define CURSOR_CURSOR_HH

// Standard C
#include <stdint.h>


namespace v68k   {
namespace cursor {

/*
	The first three fields (`face`, `mask`, and `hotspot`) correspond
	to QuickDraw's Cursor struct type.  The `seed` field is updated
	whenever any of those three change.  Typically, this indicates
	that the cursor textures (if any) require updating.
	
	(A theoretical exception to this is if only the hotspot changes.
	This technically is equivalent to changing the cursor location
	in terms of rendering requirements, but in practice it would be
	*extremely* unusual (and thus not worth trying to optimize).)
	
	The `pointer` and `visible` fields can change independently and
	act as their own seeds, in effect.  Changes to `visible` always
	require rerendering (unless the cursor face and mask happen to
	consist entirely of zero bits and you actually made the effort
	to notice this).  For all other fields (including the texture
	inputs), rerendering is only needed when the cursor is visible
	(though the texture updates are still necessary, of course).
	
	(It's theoretically valid to defer cursor texture updates for
	an invisible cursor until it becomes visible, but because it's
	pointless for an application to make frequent image changes to
	a hidden cursor, it's highly unlikely.  The vast majority of
	changes during a cursor's invisibility will be movements --
	which clients *should* take care not to render unnecessarily.)
	
	The `payload` field is experimental and for private use only.
*/

struct shared_cursor_state
{
	uint16_t face[ 16 ];
	uint16_t mask[ 16 ];
	
	uint16_t hotspot[ 2 ];
	uint16_t pointer[ 2 ];
	
	uint16_t visible;
	uint16_t reserved1;
	uint16_t reserved2;
	uint16_t seed;
	
	uint8_t  payload[ 16 ];
};

}  // namespace cursor
}  // namespace v68k


#endif
