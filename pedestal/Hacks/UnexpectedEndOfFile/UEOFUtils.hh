/*
	
	UEOFUtils.hh
	
	Joshua Juran
	
*/

#include <Quickdraw.h>
#include <Processes.h>

#include <cstring>


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

class ThingWhichPreventsMenuItemDrawing {
public:
	ThingWhichPreventsMenuItemDrawing();
	~ThingWhichPreventsMenuItemDrawing();
private:
	GrafPtr savePort;
	GrafPort myPort;
};

ThingWhichPreventsMenuItemDrawing::ThingWhichPreventsMenuItemDrawing()
{
	GetPort(&savePort);
	OpenPort(&myPort);
	SetPort(&myPort);
	myPort.portRect.right = 0;
}

ThingWhichPreventsMenuItemDrawing::~ThingWhichPreventsMenuItemDrawing()
{
	ClosePort(&myPort);
	SetPort(savePort);
}


static bool EqualPstrings(const unsigned char* p, const unsigned char* q)
{
	return (p[0] == q[0]) && (std::memcmp(p + 1, q + 1, p[0]) == 0);
}

