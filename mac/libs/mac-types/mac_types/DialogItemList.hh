/*
	DialogItemList.hh
	-----------------
*/

#ifndef MACTYPES_DIALOGITEMLIST_HH
#define MACTYPES_DIALOGITEMLIST_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreServices/CoreServices.h>
#endif

// Mac OS
#ifndef __MACTYPES__
#include <MacTypes.h>
#endif


namespace mac   {
namespace types {
	
	typedef unsigned char UInt8;
	
	typedef char** Handle;
	
	struct DialogItem
	{
		Handle  handle;
		Rect    bounds;
		UInt8   type;
		UInt8   length;
	};
	
	struct DialogItemList_header
	{
		short       count_minus_1;
		DialogItem  first;
	};
	
	inline
	DialogItem* first_dialog_item( Handle items )
	{
		return &((DialogItemList_header*) *items)->first;
	}
	
	inline
	DialogItem* next_dialog_item( DialogItem* di )
	{
		return (DialogItem*) ((char*) (di + 1) + di->length + (di->length & 1));
	}
	
	inline
	const DialogItem* next_dialog_item( const DialogItem* di )
	{
		return next_dialog_item( const_cast< DialogItem* >( di ) );
	}
	
	inline
	DialogItem* get_nth_item_inline( Handle items, short i )
	{
		DialogItem* item = first_dialog_item( items );
		
		while ( --i > 0 )
		{
			item = next_dialog_item( item );
		}
		
		return item;
	}
	
}
}

#endif
