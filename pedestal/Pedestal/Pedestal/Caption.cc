/*	==========
 *	Caption.cc
 *	==========
 */

#include "Pedestal/Caption.hh"

// Nucleus
#include "Nucleus/Saved.h"

// Nitrogen
#include "Nitrogen/TextEdit.hh"


namespace Nitrogen
{
	
	class TextMode_Value
	{
		private:
			CGrafPtr itsPort;
		
		public:
			typedef short         Value;
			typedef short         GetResult;
			typedef short const&  SetParameter;
			
			static const bool hasSwap = false;
			
			GetResult Get() const  { return GetPortTextMode( GetQDGlobalsThePort() ); }
			
			void Set( SetParameter mode ) const  { TextMode( mode ); }
	};
	
}

namespace Pedestal
{
	
	namespace N = Nitrogen;
	namespace NN = Nucleus;
	
	
	void Caption::Draw( const Rect& bounds, bool erasing )
	{
		NN::Saved< N::Clip > savedClip;
		
		Rect newBounds = bounds;
		
		if ( !Wrapped() )
		{
			newBounds.right = 30000;
			
			N::ClipRect( bounds );
		}
		
		NN::Saved< N::TextMode_Value > savedTextMode;
		
		if ( Disabled() )
		{
			::TextMode( grayishTextOr );
		}
		
		N::TETextBox( Text(), newBounds );
	}
	
}

