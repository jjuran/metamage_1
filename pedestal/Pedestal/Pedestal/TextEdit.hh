/*	===========
 *	TextEdit.hh
 *	===========
 */

#ifndef PEDESTAL_TEXTEDIT_HH
#define PEDESTAL_TEXTEDIT_HH

// Nitrogen
#include "Nitrogen/TextEdit.hh"

// Pedestal
#include "Pedestal/CustomTEClickLoop.hh"
#include "Pedestal/IncrementalSearch.hh"
#include "Pedestal/MenuItemCode.hh"
#include "Pedestal/Scroller_beta.hh"


namespace Pedestal
{
	
	class NotEnoughSpaceForTEKey;
	class NotEnoughSpaceForTEPaste;
	
	
	void Preflight_TEKey( char c, TEHandle hTE );
	
	void Preflight_TEPaste( TEHandle hTE, size_t scrapLength );
	
	
	class TextEdit : public View,
	                 public TEClickLoop_Subject,
	                 public IncrementalSearchEditor
	{
		private:
			virtual TEHandle Get() const = 0;
			
			virtual bool IsSecret  () const  { return false; }
			virtual bool IsSingular() const  { return false; }
			
			virtual void Apply_Modified_Arrow( char c, bool cmdKeyIsDown, bool optionKeyIsDown );
			
			virtual void SetActive( bool activating )  {}
			
			virtual void Insert_Key( char c );
			
			virtual void Delete();
			
			virtual void Cut();
			virtual void Paste();
			
			void Apply_Key( const EventRecord& event );
		
		public:
			virtual void On_UserSelect()  {}
			virtual void On_UserEdit()  {}
			
			virtual bool Preprocess_Key ( const EventRecord& event );
			virtual bool Process_Key    ( const EventRecord& event );
			virtual void Postprocess_Key( const EventRecord& event )  {}
			
			void Activate( bool activating );
			
			void Focus();
			void Blur ();
			void Cue  ();
			
			View* AdvanceFocus( View* current, bool backward );
			
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
	
}

#endif

