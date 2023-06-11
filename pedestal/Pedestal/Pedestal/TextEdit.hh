/*	===========
 *	TextEdit.hh
 *	===========
 */

#ifndef PEDESTAL_TEXTEDIT_HH
#define PEDESTAL_TEXTEDIT_HH

// mac-app-utils
#include "mac_app/textedit.hh"

// nucleus
#include "nucleus/owned.hh"

// Nitrogen
#ifndef MAC_TEXTEDIT_TYPES_TEHANDLE_HH
#include "Mac/TextEdit/Types/TEHandle.hh"
#endif

// Pedestal
#ifndef PEDESTAL_COMMANDCODE_HH
#include "Pedestal/CommandCode.hh"
#endif
#ifndef PEDESTAL_INCREMENTALSEARCH_HH
#include "Pedestal/IncrementalSearch.hh"
#endif
#ifndef PEDESTAL_SCROLLERBETA_HH
#include "Pedestal/Scroller_beta.hh"
#endif


namespace Pedestal
{
	
	class NotEnoughSpaceForTEKey;
	class NotEnoughSpaceForTEPaste;
	
	
	void Preflight_TEKey( char c, TEHandle hTE );
	
	void Preflight_TEPaste( TEHandle hTE );
	
	
	class TextEdit : public View
	{
		private:
			nucleus::owned< TEHandle > itsTE;
			
			TextSelection  itsSelectionPriorToSearch;
			
			virtual bool IsSecret  () const  { return false; }
			virtual bool IsSingular() const  { return false; }
			
			virtual void Apply_Modified_Arrow( char c, bool cmdKeyIsDown, bool optionKeyIsDown );
			
			virtual void SetActive( bool activating )  {}
			
			virtual void Insert_Key( char c );
			
			virtual void Delete();
			
			virtual void Cut();
			virtual void Paste();
			
			void Apply_Key( const EventRecord& event );
		
		protected:
			typedef mac::app::TEClickLoop_callback TEClickLoop_callback;
			
			TEClickLoop_callback its_TEClickLoop_callback;
		
		public:
			TextEdit() : its_TEClickLoop_callback()
			{
				itsSelectionPriorToSearch.undefine();
			}
			
			virtual void On_UserSelect()  {}
			virtual void On_UserEdit()  {}
			
			virtual bool Preprocess_Key ( const EventRecord& event );
			virtual bool Process_Key    ( const EventRecord& event );
			virtual void Postprocess_Key( const EventRecord& event )  {}
			
			TEHandle Get() const  { return itsTE; }
			
			void Install( const Rect& bounds );
			
			void Uninstall();
			
			void Activate( bool activating );
			
			void Focus();
			void Blur ();
			void Cue  ();
			
			View* AdvanceFocus( View* current, bool backward );
			
			void Idle     ( const EventRecord& event );
			bool MouseDown( const EventRecord& event );
			bool KeyDown  ( const EventRecord& event );
			
			TextEdit*
			//
			EnterShiftSpaceQuasimode( const EventRecord& event );
			
			bool SetCursor( const EventRecord& event );
			
			void Draw( const Rect& bounds, bool erasing );
			
			bool UserCommand( CommandCode code );
			
			virtual void BeginQuasimode() = 0;
			virtual void EndQuasimode() = 0;
			
			TextSelection GetCurrentSelection() const;
			
			void SetCurrentSelection( const TextSelection& selection )
			{
				Select( selection.start, selection.end );
			}
			
			TextSelection GetPriorSelection() const;
			
			void SetPriorSelection( const TextSelection& selection );
			
			void Select( unsigned start, unsigned end );
			
			void AugmentSelection();
			
			int Search( const char*           pattern,
			            std::size_t           pattern_length,
			            const TextSelection&  selection,
			            bool                  searchBackwards,
			            bool                  matchAtPosition ) const;
			
	};
	
	unsigned GetTextEditingHeight( const TERec& te );
	
}

#endif
