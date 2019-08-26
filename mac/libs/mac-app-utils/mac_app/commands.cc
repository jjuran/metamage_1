/*
	commands.hh
	-----------
*/

#include "mac_app/commands.hh"

// mac-app-utils
#include "mac_app/hooks.hh"
#include "mac_app/state.hh"


namespace mac {
namespace app {

long quit()
{
	if ( quit_hook )
	{
		if ( long err = quit_hook() )
		{
			return err;
		}
	}
	
	quitting = true;
	
	return 0;
}

}
}
