/*
	Desk.cc
	-------
*/

#include "Desk.hh"

// macos
#include "splode.hh"


pascal void SystemTask_patch()
{
	poll_user_input();
}
