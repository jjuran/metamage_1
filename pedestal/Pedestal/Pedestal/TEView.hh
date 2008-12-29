/*	=========
 *	TEView.hh
 *	=========
 */

#ifndef PEDESTAL_TEVIEW_HH
#define PEDESTAL_TEVIEW_HH

// Nitrogen
#include "Nitrogen/TextEdit.h"

// Pedestal
#include "Pedestal/MenuItemCode.hh"
#include "Pedestal/View.hh"


namespace Pedestal
{
	
	struct TESelection
	{
		short start;
		short end;
	};
	
	Rect Bounds( TEHandle hTE );
	short CountLinesForDisplay( TEHandle hTE );
	short CountLinesForEditing( TEHandle hTE );
	Point ScrollStep( TEHandle hTE );
	
	void Resize( TEHandle hTE, short width, short height );
	
	
	class TESearchQuasimode;
	
	class TEView : public View
	{
		friend class TESearchQuasimode;
		
		private:
			Nucleus::Owned< TEHandle > itsTE;
			
			TESelection itsSelectionPriorToSearch;
			TESelection itsSelectionPriorToArrow;
		
		public:
			TEView( const Rect& bounds );
			
			TEHandle Get() const  { return itsTE; }
			
			Rect Bounds() const  { return Pedestal::Bounds( Get() ); }
			
			// Text manipulation
			SInt16 TextLength() const  { return itsTE[0]->teLength; }
			Handle TextHandle()        { return itsTE[0]->hText;    }
			
			void SetSelection( short start, short end )
			{
				Nitrogen::TESetSelect( start, end, itsTE );
			}
			
			void SetSelection( const TESelection& selection )
			{
				SetSelection( selection.start, selection.end );
			}
			
			int AppendChars( const char* data, unsigned int byteCount, bool updateNow );
			
			void Cut();
			void Copy();
			void Paste();
			void Clear();
			void SelectAll();
			
			Point ViewableRange() const;
			Point ScrollableRange() const;
			Point ScrollPosition() const;
			
			void Scroll( short dh, short dv, bool updateNow );
			
			void Idle     ( const EventRecord& event );
			void MouseDown( const EventRecord& event );
			bool KeyDown  ( const EventRecord& event );
			
			boost::shared_ptr< Quasimode > EnterShiftSpaceQuasimode( const EventRecord& event );
			
			void Activate( bool activating );
			
			void Draw( const Rect& bounds );
			
			void Resize( short width, short height )  { Pedestal::Resize( Get(), width, height ); }
			
			bool SetCursor( const EventRecord& event, RgnHandle mouseRgn );
			
			bool UserCommand( MenuItemCode code );
	};
	
}

#endif

