/*
	
	TECalc
	
	Joshua Juran
	
*/

// Universal Interfaces
#include <Events.h>
#include <NumberFormatting.h>
#include <Sound.h>
#include <TextEdit.h>
#include <Traps.h>

// Standard C/C++
#include <cstring>

// Silver
#include "Silver/Patches.hh"


using Silver::PatchApplied;
using Silver::TEKeyPatch;

static PatchApplied<TEKeyPatch> gTEKeyPatch;


extern long expression(const char* expr);

static pascal void PatchedTEKey(short c, TEHandle hTE);

static void Payload(TEHandle hTE)
{
	long start = (**hTE).selStart;
	long end   = (**hTE).selEnd;
	long len = end - start;
	
	if ( len > 255 )
	{
		SysBeep( 30 );
		return;
	}
	
	char buf[256];
	
	std::memcpy(buf, *(**hTE).hText, len);
	buf[len] = '\0';
	long value = expression(buf);
	NumToString((short)value, (unsigned char*)buf);
	
	TEDelete(hTE);
	TEInsert(buf + 1, buf[0], hTE);
	TESetSelect(start, start + buf[0], hTE);
}

static pascal void PatchedTEKey(short c, TEHandle hTE)
{
	MyA4 a4;
	
	long start = (**hTE).selStart;
	long end   = (**hTE).selEnd;
	
	if ((c == '=') && (start != end)) {
		Payload(hTE);
	} else {
		gTEKeyPatch.Original()(c, hTE);
	}
}

static bool Install()
{
	bool locked = LoadAndLock();
	if (!locked)
		return false;
	
	MyA4 a4;
	
	gTEKeyPatch = TEKeyPatch(PatchedTEKey);
	
	return true;
}

void main()
{
	bool installed = Install();
}

