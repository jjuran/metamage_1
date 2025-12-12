/*
	callback.cc
	-----------
*/

#include "vxo/callback.hh"


namespace vxo
{

bool no_duplication( void* )
{
	return false;
}

bool POD_duplicator( void* )
{
	return true;
}

}
