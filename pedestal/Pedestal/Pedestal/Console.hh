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
			bool pendingCR;
		
		public:
			Console( const Rect& bounds, const Initializer& init )
			:
				TEView( bounds, init ),
				pendingCR( false )
			{}
			
			int WriteChars( const char* data, unsigned int byteCount );
	};
	
}

#endif

