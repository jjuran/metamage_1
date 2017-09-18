/*
	MDEF.hh
	-------
*/

#ifndef MDEF_HH
#define MDEF_HH

struct MenuInfo;
struct Point;
struct Rect;

void MDEF_0( short msg, MenuInfo** menu, const Rect* r, Point pt, short* item );

class menu_item_iterator
{
	private:
		unsigned char* next;
	
	public:
		menu_item_iterator( MenuInfo** menu )
		{
			next = (unsigned char*) *menu + 14;  // menu[0]->menuData;
			
			next += 1 + next[ 0 ];
		}
		
		const unsigned char* get() const  { return next; }
		
		// bool conversion
		operator unsigned char*() const  { return *next ? next : 0; }
		
		menu_item_iterator& operator++()
		{
			next += 1 + next[ 0 ] + 4;
			return *this;
		}
		
		menu_item_iterator operator++( int )
		{
			menu_item_iterator prev = *this;
			
			++*this;
			
			return prev;
		}
};

#endif
