/*
	menu_iterator.hh
	----------------
*/

#ifndef MENUITERATOR_HH
#define MENUITERATOR_HH


struct MenuInfo;

typedef unsigned char Byte;

typedef MenuInfo** MenuRef;

inline
bool small_icon_key( Byte key )
{
	return key == 0x1D;
}

inline
bool large_icon_key( Byte key )
{
	return ! small_icon_key( key );
}

class menu_iterator
{
	private:
		Byte* next;
	
	public:
		menu_iterator( MenuRef menu )
		{
			next = (Byte*) *menu + 14;  // menu[0]->menuData;
			
			next += 1 + next[ 0 ];
		}
		
		const Byte* get() const  { return next; }
		
		// bool conversion
		operator Byte*() const  { return *next ? next : 0; }
		
		menu_iterator& operator++()
		{
			next += 1 + next[ 0 ] + 4;
			
			return *this;
		}
		
		menu_iterator operator++( int )
		{
			menu_iterator prev = *this;
			
			++*this;
			
			return prev;
		}
};

#endif
