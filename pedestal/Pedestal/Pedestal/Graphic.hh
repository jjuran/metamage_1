/*	==========
 *	Graphic.hh
 *	==========
 */

#ifndef PEDESTAL_GRAPHIC_HH
#define PEDESTAL_GRAPHIC_HH

#if PRAGMA_ONCE
#pragma once
#endif


namespace Pedestal
{
	
	class Graphic
	{
		public:
			struct Initializer {};
			
			void Plot( const Rect& /*area*/ )  {}
	};
	
}

#endif

