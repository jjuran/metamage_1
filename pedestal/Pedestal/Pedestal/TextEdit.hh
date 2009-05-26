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
			
			virtual bool Preprocess_Key ( const EventRecord& event );
			virtual bool Process_Key    ( const EventRecord& event );
			virtual void Postprocess_Key( const EventRecord& event )  {}
			
			void Activate( bool activating );
			
			void Idle     ( const EventRecord& event );
			bool MouseDown( const EventRecord& event );
			bool KeyDown  ( const EventRecord& event );
			
			boost::shared_ptr< Quasimode >
			//
			EnterShiftSpaceQuasimode( const EventRecord& event );
			
			bool SetCursor( const EventRecord&  event,
			                RgnHandle           mouseRgn );
			
			void Draw( const Rect& bounds, bool erasing );
			
			bool UserCommand( MenuItemCode code );
			
			TextSelection GetCurrentSelection() const;
			
			void Select( unsigned start, unsigned end );
			
			int Search( const std::string&    pattern,
			            const TextSelection&  selection,
			            bool                  searchBackwards,
			            bool                  matchAtPosition ) const;
			
	};
	
	unsigned GetTextEditingHeight( const TERec& te );
	
	bool KeyIsPreHandled( TextEdit& that, const EventRecord& event );
	
}

#endif

