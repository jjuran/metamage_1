/*	==========
 *	Caption.hh
 *	==========
 */

#ifndef PEDESTAL_CAPTION_HH
#define PEDESTAL_CAPTION_HH

// Standard C++
#include <string>

// Pedestal
#include "Pedestal/Graphic.hh"


namespace Pedestal
{
	
	class Caption : public Graphic
	{
		public:
			virtual std::string Text() const = 0;
			
			void Plot( const Rect& area );
	};
	
}

#endif

