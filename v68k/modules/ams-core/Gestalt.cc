/*
	Gestalt.cc
	----------
*/

#include "Gestalt.hh"


enum
{
	gestaltUserVisibleMachineName = 'mnam',
};

Gestalt_ProcPtr old_Gestalt;

pascal short Gestalt_patch( unsigned long  selector : __D0,
                            long*          response : __A1 ) : __D0
{
	switch ( selector )
	{
		case gestaltUserVisibleMachineName:
			*(unsigned char**) response = "\p" "Advanced Mac Substitute";
			break;
		
		default:
			return old_Gestalt( selector, response );
	}
	
	return 0;
}
