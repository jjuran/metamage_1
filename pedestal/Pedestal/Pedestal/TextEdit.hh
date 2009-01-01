/*	===========
 *	TextEdit.hh
 *	===========
 */

#ifndef PEDESTAL_TEXTEDIT_HH
#define PEDESTAL_TEXTEDIT_HH

// Nitrogen
#include "Nitrogen/TextEdit.h"

// Pedestal
#include "Pedestal/CustomTEClickLoop.hh"
#include "Pedestal/IncrementalSearch.hh"
#include "Pedestal/MenuItemCode.hh"
#include "Pedestal/Scroller_beta.hh"


namespace Pedestal
{
	
	class TextEdit : public View,
	                 public TEClickLoop_Subject,
	                 public IncrementalSearchEditor
	{
		private:
			virtual TEHandle Get() const = 0;
			
			View itsNullView;
			
			View& Subview()  { return itsNullView; }
		
		public:
			virtual void On_UserSelect()  {}
			virtual void On_UserEdit()  {}
			
			virtual void On_EnterKey()  {}
			
			void Activate( bool activating );
			
			void Idle     ( const EventRecord& event );
			void MouseDown( const EventRecord& event );
			bool KeyDown  ( const EventRecord& event );
			
			bool SetCursor( const EventRecord&  event,
			                RgnHandle           mouseRgn );
			
			void Draw( const Rect& bounds );
			
			bool UserCommand( MenuItemCode code );
			
			TextSelection GetCurrentSelection() const;
			
			void Select( unsigned start, unsigned end );
			
			int Search( const std::string&    pattern,
			            const TextSelection&  selection,
			            bool                  searchBackwards,
			            bool                  matchAtPosition ) const;
			
	};
	
}

#endif

