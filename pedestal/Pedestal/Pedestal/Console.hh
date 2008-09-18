/*	==========
 *	Console.hh
 *	==========
 */

#ifndef PEDESTAL_CONSOLE_HH
#define PEDESTAL_CONSOLE_HH

#if PRAGMA_ONCE
#pragma once
#endif

#include "Pedestal/TEView.hh"


namespace Pedestal
{
	
	class Console : public TEView
	{
		private:
			bool itHasPendingCR;
		
		public:
			Console( const Rect&         bounds,
			         const Initializer&  init ) : TEView        ( bounds, init ),
			                                      itHasPendingCR( false        )
			{
			}
			
			int WriteChars( const char* data, unsigned int byteCount );
	};
	
}

#endif

