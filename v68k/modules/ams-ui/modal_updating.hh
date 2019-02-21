/*
	modal_updating.hh
	-----------------
*/

#ifndef MODALUPDATING_HH
#define MODALUPDATING_HH

extern int exclusive_modal_updating;

class modal_update_scope
{
	private:
		bool modal_dialog_lacks_filterProc;
		
		// non-copyable
		modal_update_scope           ( const modal_update_scope& );
		modal_update_scope& operator=( const modal_update_scope& );
	
	public:
		modal_update_scope( bool lax ) : modal_dialog_lacks_filterProc( lax )
		{
			exclusive_modal_updating += modal_dialog_lacks_filterProc;
		}
		
		~modal_update_scope()
		{
			exclusive_modal_updating -= modal_dialog_lacks_filterProc;
		}
};

#endif
