/*
	menu_iterator.hh
	----------------
*/

#ifndef MENUITERATOR_HH
#define MENUITERATOR_HH


enum
{
	kMenuItemKey_icon_reduction = 0x1D,
	kMenuItemKey_icon_from_SICN = 0x1E,
};

struct MenuInfo;

typedef unsigned char Byte;

typedef MenuInfo** MenuRef;

inline
bool large_icon_key( Byte key )
{
	return key - 0x1D >> 1;  // neither $1D nor $1E
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
