/*
	admin.hh
	--------
*/

#ifndef ADMIN_HH
#define ADMIN_HH


enum
{
	kSoundDriverRefNum = -4,
};

enum
{
	/*
		Apparently noHardwareErr isn't used for anything -- at least
		nothing deserving of mention in Apple's Sound Manager 3.3
		documentation -- so we'll use it here for things that are
		yet to be implemented.  (There's also unimpErr, but that's
		used to indicate that a particular software operation is
		unsupported by a hardware device, e.g. volume control.)
	*/
	
	unimplemented = -200,  // same as noHardwareErr
};

struct SndChannel;
struct SndCommand;

typedef short OSErr;

OSErr do_admin_command( SndChannel* chan, SndCommand& command );

#endif
