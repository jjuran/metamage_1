/*
	admin.cc
	--------
*/

#include "admin.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __SOUND__
#include <Sound.h>
#endif

// ams-common
#include "interrupts.hh"


static inline
OSErr do_quietCmd( SndChannel* chan )
{
	return KillIO( kSoundDriverRefNum );
}

static inline
OSErr do_flushCmd( SndChannel* chan )
{
	short saved_SR = disable_interrupts();
	
	chan->qHead =  0;
	chan->qTail = -1;
	
	reenable_interrupts( saved_SR );
	
	return 0;
}

OSErr do_admin_command( SndChannel* chan, SndCommand& command )
{
	switch ( command.cmd )
	{
		case nullCmd:
			return noErr;
		
		case quietCmd:
			return do_quietCmd( chan );
		
		case flushCmd:
			return do_flushCmd( chan );
		
		case callBackCmd:
			if ( SndCallBackUPP callBack = chan->callBack )
			{
				callBack( chan, &command );
			}
			break;
		
		default:
			return unimplemented;
	}
	
	return noErr;
}
