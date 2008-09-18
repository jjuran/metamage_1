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

#if PRAGMA_ONCE
#pragma once
#endif


namespace Pedestal
{
	
	class Caption : public Graphic
	{
		private:
			std::string text;
		
		public:
			typedef std::string Initializer;
			
			Caption( const std::string& text ) : text( text )  {}
			
			std::string Text() const  { return text; }
			
			void SetText( const std::string& newText )  { text = newText; }
			
			void Plot( const Rect& area );
	};
	
}

#endif

