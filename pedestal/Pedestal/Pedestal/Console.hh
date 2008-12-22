/*	==========
 *	Console.hh
 *	==========
 */

#ifndef PEDESTAL_CONSOLE_HH
#define PEDESTAL_CONSOLE_HH

#include "Pedestal/TEView.hh"


namespace Pedestal
{
	
	class Console : public TEView
	{
		private:
			bool itHasPendingCR;
		
		public:
			Console( const Rect& bounds ) : TEView        ( bounds ),
			                                itHasPendingCR( false  )
			{
			}
			
			int WriteChars( const char* data, unsigned int byteCount );
	};
	
}

#endif

