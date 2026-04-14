/*
	commandmode_state.hh
	--------------------
*/

#ifndef FREND_COMMANDMODESTATE_HH
#define FREND_COMMANDMODESTATE_HH


namespace frend
{

/*
	Command mode is a means of distinguishing between user input to be
	forwarded to the back end application and that to be intepreted by
	the front end.  Pressing a combination of modifier keys activates the
	mode; releasing them without having pressed any other keys enters a
	one-shot mode that interprets the next keystroke as a command and then
	switches back to pass-through (deactivating command mode).  If other
	keys are pressed before releasing all the modifier keys, they are all
	interpreted as commands, and releasing the modifier keys will deactivate
	command mode immediately.
*/

enum CommandMode_state
{
	CommandMode_quasimode = -2,
	CommandMode_activated,
	CommandMode_off,
	CommandMode_oneshot,
};

extern CommandMode_state commandmode_state;

}

#endif
