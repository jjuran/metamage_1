/*
	textedit.hh
	-----------
*/

#ifndef MACAPP_TEXTEDIT_HH
#define MACAPP_TEXTEDIT_HH

// iota
#include "iota/class.hh"

// <TextEdit.h>
struct TERec;
typedef TERec** TEHandle;


namespace mac {
namespace app {
	
	void install_custom_TEClickLoop( TEHandle hTE );
	
	typedef void (*TEClickLoop_callback)(void* data);
	
	extern TEClickLoop_callback  current_TEClick_scroller_code;
	extern void*                 current_TEClick_scroller_data;
	extern TEClickLoop_callback  current_TEClick_contents_code;
	extern void*                 current_TEClick_contents_data;
	
	class TEClick_scroller_scope
	{
		NON_COPYABLE( TEClick_scroller_scope )
		NO_NEW_DELETE
		
		public:
			TEClick_scroller_scope(TEClickLoop_callback code, void* data)
			{
				current_TEClick_scroller_code = code;
				current_TEClick_scroller_data = data;
			}
			
			~TEClick_scroller_scope()
			{
				current_TEClick_scroller_code = 0;  // NULL
				current_TEClick_scroller_data = 0;  // NULL
			}
	};
	
	class TEClick_contents_scope
	{
		NON_COPYABLE( TEClick_contents_scope )
		NO_NEW_DELETE
		
		public:
			TEClick_contents_scope(TEClickLoop_callback code, void* data)
			{
				current_TEClick_contents_code = code;
				current_TEClick_contents_data = data;
			}
			
			~TEClick_contents_scope()
			{
				current_TEClick_contents_code = 0;  // NULL
				current_TEClick_contents_data = 0;  // NULL
			}
	};
	
}
}

#endif
