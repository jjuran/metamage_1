/*
	MDEF.hh
	-------
*/

#ifndef MDEF_HH
#define MDEF_HH

struct MenuInfo;
struct Point;
struct Rect;

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

pascal
void MDEF_0( short msg, MenuRef menu, Rect* r, Point pt, short* item );

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
