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
	
	namespace N = Nitrogen;
	
	
	Rect Bounds( TEHandle hTE );
	short CountLinesForDisplay( TEHandle hTE );
	short CountLinesForEditing( TEHandle hTE );
	short ViewableLines( TEHandle hTE );
	Point ViewableRange( TEHandle hTE );
	Point ScrollableRange( TEHandle hTE );
	Point ScrollStep( TEHandle hTE );
	Point ScrollPosition( TEHandle hTE );
	
	void Resize( TEHandle hTE, short width, short height );
	void Resize( TEHandle hTE, const Rect& newBounds );
	
	void Scroll( TEHandle hTE, short dh, short dv );
	
	
	class TEView : public View
	{
		private:
			N::Owned< TEHandle > myTE;
		
		public:
			TEView( const Rect& bounds, Initializer );
			
			TEHandle Get() const  { return myTE; }
			
			Rect Bounds() const  { return Pedestal::Bounds( Get() ); }
			
			// Text manipulation
			SInt16 TextLength() const  { return N::GetTELength( myTE ); }
			Handle TextHandle()  { return N::GetTETextHandle( myTE ); }
			
			void SetSelection( short start, short end );
			
			int AppendChars( const char* data, unsigned int byteCount );
			
			void Cut();
			void Copy();
			void Paste();
			void Clear();
			void SelectAll();
			
			void Idle     ( const EventRecord& event );
			void MouseDown( const EventRecord& event );
			bool KeyDown  ( const EventRecord& event );
			
			void Activate( bool activating );
			
			void Update();
			
			void Resize( const Rect& newBounds )  { Pedestal::Resize( Get(), newBounds ); }
			
			bool SetCursor( Point location, RgnHandle mouseRgn );
			
			bool UserCommand( MenuItemCode code );
	};
	
	inline Point ViewableRange( const TEView& view )
	{
		return ViewableRange( view.Get() );
	}
	
	inline Point ScrollableRange( const TEView& view )
	{
		return ScrollableRange( view.Get() );
	}
	
	inline Point ScrollPosition( const TEView& view )
	{
		return ScrollPosition( view.Get() );
	}
	
	inline void Scroll( TEView& view, short dh, short dv, bool /*updateNow*/ )
	{
		Scroll( view.Get(), dh, dv );
	}
	
}

#endif

