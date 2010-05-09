/*	==========
 *	Caption.hh
 *	==========
 */

#ifndef PEDESTAL_CAPTION_HH
#define PEDESTAL_CAPTION_HH

// plus
#include "plus/string.hh"

// Pedestal
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	class Caption : public View
	{
		public:
			virtual plus::string Text() const = 0;
			
			virtual bool Wrapped() const  { return true; }
			virtual bool Disabled() const  { return false; }
			
			void Draw( const Rect& bounds, bool erasing );
	};
	
}

#endif

