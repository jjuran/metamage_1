/*	=========
 *	TEView.hh
 *	=========
 */

#ifndef PEDESTAL_TEVIEW_HH
#define PEDESTAL_TEVIEW_HH

// Nitrogen
#include "Nitrogen/TextEdit.h"

// Pedestal
#include "Pedestal/CustomTEClickLoop.hh"
#include "Pedestal/IncrementalSearch.hh"
#include "Pedestal/MenuItemCode.hh"
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	Rect Bounds( TEHandle hTE );
	short CountLinesForDisplay( TEHandle hTE );
	short CountLinesForEditing( TEHandle hTE );
	Point ScrollStep( TEHandle hTE );
	
	void Resize( TEHandle hTE, short width, short height );
	
	
	class TEView : public View, public TEClickLoop_Subject, public IncrementalSearchEditor
	{
		private:
			Nucleus::Owned< TEHandle > itsTE;
			
			TextSelection itsSelectionPriorToSearch;
		
		public:
			TEView( const Rect& bounds );
			
			TEHandle Get() const  { return itsTE; }
			
			Rect Bounds() const  { return Pedestal::Bounds( Get() ); }
			
			int AppendChars( const char* data, unsigned int byteCount, bool updateNow );
			
			Point ViewableRange() const;
			Point ScrollableRange() const;
			Point ScrollPosition() const;
			
			void Scroll( int dh, int dv );
			
			void Idle     ( const EventRecord& event );
			void MouseDown( const EventRecord& event );
			bool KeyDown  ( const EventRecord& event );
			
			boost::shared_ptr< Quasimode > EnterShiftSpaceQuasimode( const EventRecord& event );
			
			void Activate( bool activating );
			
			void Draw( const Rect& bounds );
			
			void Resize( short width, short height )  { Pedestal::Resize( Get(), width, height ); }
			
			bool SetCursor( const EventRecord& event, RgnHandle mouseRgn );
			
			bool UserCommand( MenuItemCode code );
			
			void BeginQuasimode();
			void EndQuasimode();
			
			TextSelection GetCurrentSelection() const;
			
			void Select( unsigned start, unsigned end );
			
			TextSelection GetPriorSelection() const  { return itsSelectionPriorToSearch; }
			
			void SetPriorSelection( const TextSelection& selection )  { itsSelectionPriorToSearch = selection; }
			
			int Search( const std::string&    pattern,
			            const TextSelection&  selection,
			            bool                  searchBackwards,
			            bool                  matchAtPosition ) const;
	};
	
}

#endif

