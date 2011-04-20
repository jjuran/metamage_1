/*
	
	UEOFUtils.hh
	
	Joshua Juran
	
*/

#include <Quickdraw.h>
#include <Processes.h>

#include <cstring>


#pragma exceptions off


static bool operator==(const ProcessSerialNumber& a, const ProcessSerialNumber& b)
{
	return a.highLongOfPSN == b.highLongOfPSN  &&  a.lowLongOfPSN == b.lowLongOfPSN;
}

static OSErr GetCurrentProcessInfo(ProcessInfoRec& procInfo)
{
	OSErr err;
	ProcessSerialNumber psn;
	
	procInfo.processInfoLength = sizeof procInfo;
	procInfo.processName = NULL;
	procInfo.processAppSpec = NULL;
	
	err = GetFrontProcess(&psn);
	if (err) return err;
	
	err = GetProcessInformation(&psn, &procInfo);
	if (err) return err;
	
	return noErr;
}

static ProcessSerialNumber CurrentPSN()
{
	ProcessSerialNumber psn;
	
	OSErr err = GetFrontProcess(&psn);
	//if (err) return 0;
	
	return psn;
}

static short IsFinderInForeground()
{
	ProcessInfoRec procInfo;
	
	OSErr err = GetCurrentProcessInfo(procInfo);
	if (err) return 0;
	
	return (procInfo.processSignature == 'MACS');
}

class ThingWhichPreventsMenuItemDrawing
{
	private:
		GrafPtr savePort;
		GrafPort myPort;
		
		// Non-copyable
		ThingWhichPreventsMenuItemDrawing           ( const ThingWhichPreventsMenuItemDrawing& );
		ThingWhichPreventsMenuItemDrawing& operator=( const ThingWhichPreventsMenuItemDrawing& );
	
	public:
		ThingWhichPreventsMenuItemDrawing()
		{
			GetPort( &savePort );
			
			OpenPort( &myPort );
			SetPort ( &myPort );
			
			myPort.portRect.right = 0;
		}
		
		~ThingWhichPreventsMenuItemDrawing()
		{
			ClosePort( &myPort );
			
			SetPort( savePort );
		}
};


inline bool EqualPstrings( const unsigned char* p, const unsigned char* q )
{
	return std::memcmp( p, q, 1 + p[0] ) == 0;
}

