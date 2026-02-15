/*
	CastlePrefsINIT.cc.hh
	---------------------
	
	Castle Prefs INIT template for Advanced Mac Substitute
	
	Copyright 2026, Joshua Juran.  All rights reserved.
	
	License:  AGPLv3+ (see bottom for legal boilerplate)
	
	This program enables user-writable preferences
	for either Dark Castle or Beyond Dark Castle.
	
	It runs at startup (but doesn't install any patches).
	If the Preferences volume is missing the appropriate
	preferences file, a copy is made from the game disk.
	
	When the game opens its preferences file, the copy on
	the (writable) Preferences volume will match first.
	
*/

// Mac OS
#ifndef __DEVICES__
#include <Devices.h>
#endif
#ifndef __FILES__
#include <Files.h>
#endif


static Str27 vol_name = "\p" "Preferences";
static Str31 filename = "\p" PREFS_FILENAME;

/*
	We use two parameter blocks.  We could have used only one
	if the second GetFInfo call followed write_file() instead
	of preceding it, but it would require knowing the vRefNum
	of the game disk in order to get info specifically of the
	original instead of the copy (which is the entire point).
	Determining the vRefNum is non-trivial, however, since we
	don't know the name of the game disk (which could even be
	the bootstrap volume).  So we'd have to get the file info
	of the original before creating the new file anyway, only
	to have to do so again afterward (or copy the Finder info
	elsewhere and then back).  Just use two parameter blocks.
*/

static ParamBlockRec pb_data;
static ParamBlockRec pb_info;

/*
	This is the sequence of operations:
	
	1.  info    GetVInfo
	2.  info    GetFInfo
	3.  info    GetFInfo
	4.  data    OpenRF
	5.  data    Read
	6.  data    Close
	7.  data    Create
	8.  data    OpenRF
	9.  data    Write
	10. data    Close
	11. info    SetFInfo
*/

static inline
bool has_Preferences()
{
	OSErr err;
	
	VolumeParam& vol = pb_info.volumeParam;
	
	vol.ioNamePtr  = vol_name;
//	vol.ioVRefNum  = 0;         // initialized to zero
	vol.ioVolIndex = -1;
	
	err = PBGetVInfoSync( &pb_info );  // #1
	
	return err == noErr;
}

static inline
bool needs_prefs_file()
{
	OSErr err;
	
	FileParam& file = pb_info.fileParam;
	
	file.ioNamePtr   = filename;
//	file.ioVRefNum   = vRefNum;  // already set in has_Preferences()
//	file.ioFVersNum  = 0;        // initialized to zero
	file.ioFDirIndex = 0;
	
	err = PBGetFInfoSync( &pb_info );  // #2
	
	return err == fnfErr;
}

static inline
OSErr read_file()
{
	OSErr err;
	
	Size size = pb_info.fileParam.ioFlRLgLen;  // from GetFInfo
	
	IOParam& io = pb_data.ioParam;
	
	io.ioNamePtr = filename;
//	io.ioVRefNum = 0;         // initialized to zero
//	io.ioVersNum = 0;         // initialized to zero
//	io.ioPermssn = 0;         // initialized to zero
//	io.ioMisc    = 0;         // initialized to zero
	
	err = PBOpenRFSync( &pb_data );  // #4
	
	Ptr buffer;
	
	if ( err == noErr  &&  (buffer = NewPtr( size )) )
	{
//		io.ioRefNum    = refnum;    // set by Open
		io.ioBuffer    = buffer;
		io.ioReqCount  = size;
		io.ioPosMode   = fsAtMark;
//		io.ioPosOffset = 0;         // not used with fsAtMark
		
		err = PBReadSync( &pb_data );  // #5
		
		if ( err )
		{
			DisposePtr( buffer );
		}
		
//		io.ioRefNum = refnum;  // set by Open
		
		PBCloseSync( &pb_data );  // #6
	}
	
	return err;
}

static inline
OSErr write_file( short vRefNum )
{
	OSErr err;
	
	IOParam& io = pb_data.ioParam;
	
	FileParam& file = pb_data.fileParam;
	
	file.ioVRefNum = vRefNum;
	file.ioNamePtr = filename;
	
	err = PBCreateSync( &pb_data );  // #7
	
	if ( err == noErr )
	{
//		io.ioNamePtr = filename;  // set above
//		io.ioVRefNum = vRefNum;   // set above
//		io.ioVersNum = 0;         // initialized to zero
//		io.ioPermssn = 0;         // initialized to zero
//		io.ioMisc    = 0;         // initialized to zero
		
		err = PBOpenRFSync( &pb_data );  // #8
		
		if ( err == noErr )
		{
//			io.ioRefNum    = refnum;         // set by Open
//			io.ioBuffer    = buffer;         // set in read_file()
			io.ioReqCount  = io.ioActCount;  // set by Read
			io.ioPosMode   = fsAtMark;
//			io.ioPosOffset = 0;              // not used with fsAtMark
			
			err = PBWriteSync( &pb_data );  // #9
			
//			io.ioRefNum = refnum;  // set by Open
			
			PBCloseSync( &pb_data );  // #10
		}
	}
	
	DisposePtr( io.ioBuffer );
	
	return err;
}

int main()
{
	OSErr err;
	
	if ( has_Preferences()  &&  needs_prefs_file() )
	{
		/*
			The file wasn't found on the Preferences
			volume.  Copy it from the disk image.
		*/
		
		FileParam& file = pb_info.fileParam;
		
		short prefs_vRefNum = file.ioVRefNum;
		
//		file.ioNamePtr  = filename;       // set previously
		file.ioVRefNum  = 0;
//		file.ioFVersNum = 0;              // initialized to zero
		
		err = PBGetFInfoSync( &pb_info );  // #3
		
		if ( err == noErr )
		{
			err = read_file();
			
			if ( err == noErr )
			{
				err = write_file( prefs_vRefNum );
				
				if ( err == noErr )
				{
//					file.ioNamePtr  = filename;       // set previously
					file.ioVRefNum  = prefs_vRefNum;
//					file.ioFVersNum = 0;              // initialized to zero
					
					err = PBSetFInfoSync( &pb_info );  // #11
				}
			}
		}
	}
	
	return 0;
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
