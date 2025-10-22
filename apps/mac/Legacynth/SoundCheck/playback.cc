/*
	playback.cc
	-----------
	
	Sound Check for classic Mac OS
	
	Copyright 2025, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	Sound Check tests the Sound Driver API.
	
	This module implements asynchronous audio playback via the Sound Driver.
	
*/

#include "playback.hh"

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __PROCESSES__
#include <Processes.h>
#endif
#ifndef __RESOURCES__
#include <Resources.h>
#endif

// mac-glue-utils
#include "mac_glue/Memory.hh"

// mac-sys-utils
#include "mac_sys/has/ProcessManager.hh"
#include "mac_sys/res_error.hh"


#pragma exceptions off


enum
{
	kFlatSoundResType = '_SND',
};

enum
{
	kFlatSoundResID = 128,
};

enum
{
	kSoundDriverRefNum = -4,
};

volatile bool playing;
volatile bool updated;

static Handle sound;

static ParamBlockRec pb;

static ProcessSerialNumber psn;

static
OSErr audio_completion( ParamBlockRec* pb : __A0 )
{
	playing = false;
	updated = true;
	
	if ( psn.lowLongOfPSN )
	{
		WakeUpProcess( &psn );
	}
	else
	{
		PostEvent( app1Evt, 0 );
	}
	
	return noErr;
}

OSErr start_playback()
{
	if ( playing )
	{
		return fBsyErr;
	}
	
	OSErr err;
	
	if ( ! sound  ||  ! *sound )
	{
		sound = GetResource( kFlatSoundResType, kFlatSoundResID );
		
		if ( ! sound )
		{
			if ( ! (err = mac::sys::res_error()) )
			{
				err = resNotFound;
			}
			
			return err;
		}
		
		if ( ! *sound )
		{
			return memFullErr;
		}
	}
	
	if ( psn.lowLongOfPSN == 0  &&  mac::sys::has_ProcessManager() )
	{
		GetCurrentProcess( &psn );
	}
	
	HLock( sound );
	
	pb.ioParam.ioRefNum   = kSoundDriverRefNum;
	pb.ioParam.ioBuffer   = *sound;
	pb.ioParam.ioReqCount = mac::glue::GetHandleSize_raw( sound );
	
	err = PBKillIOSync( &pb );
	
	pb.ioParam.ioCompletion = (IOCompletionProcPtr) &audio_completion;
	
	err = PBWriteAsync( &pb );
	
	playing = err == noErr;
	
	return err;
}

OSErr stop_playback()
{
	playing = false;
	
	HUnlock( sound );
	
	pb.ioParam.ioRefNum = kSoundDriverRefNum;
	
	return PBKillIOSync( &pb );
}

/*
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU Affero General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU Affero General Public License for more details.

    You should have received a copy of the GNU Affero General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
