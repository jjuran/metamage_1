/*	=========
 *	TEView.hh
 *	=========
 */

#ifndef PEDESTAL_TEVIEW_HH
#define PEDESTAL_TEVIEW_HH

// Pedestal
#include "Pedestal/TextEdit.hh"


namespace Pedestal
{
	
	Rect Bounds( TEHandle hTE );
	short CountLinesForDisplay( TEHandle hTE );
	short CountLinesForEditing( TEHandle hTE );
	Point ScrollStep( TEHandle hTE );
	
	void Resize( TEHandle hTE, short width, short height );
	
	
	class TEView : public TextEdit
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
			Point ScrollUnits() const;
			
			void Scroll( int dh, int dv );
			
			boost::shared_ptr< Quasimode > EnterShiftSpaceQuasimode( const EventRecord& event );
			
			void Draw( const Rect& bounds );
			
			void Resize( short width, short height )  { Pedestal::Resize( Get(), width, height ); }
			
			void BeginQuasimode();
			void EndQuasimode();
			
			TextSelection GetPriorSelection() const  { return itsSelectionPriorToSearch; }
			
			void SetPriorSelection( const TextSelection& selection )  { itsSelectionPriorToSearch = selection; }
	};
	
}

#endif

