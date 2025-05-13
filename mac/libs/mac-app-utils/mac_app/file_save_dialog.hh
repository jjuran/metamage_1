/*
	file_save_dialog.hh
	-------------------
*/

#ifndef MACAPP_FILESAVEDIALOG_HH
#define MACAPP_FILESAVEDIALOG_HH

// Mac OS X
#ifdef __APPLE__
#include <CoreFoundation/CoreFoundation.h>
#endif

// Mac OS
#ifndef __COREFOUNDATION_CFSTRING__
#ifndef __CFSTRING__
#include <CFString.h>
#endif
#endif

struct FSRef;
struct FSSpec;

typedef unsigned char Byte;

namespace mac {
namespace app {
	
	typedef unsigned long OSType;
	typedef const Byte* StrArg;
	
	typedef long (*HFS_callback)( short vRefNum, long dirID, const Byte* name );
	
	typedef long (*FSSpec_callback)( const FSSpec& file );
	
	typedef HFS_callback    HFS_Proc;
	typedef FSSpec_callback FSS_Proc;
	
	typedef long (*FSRefName_callback )( const FSRef& file, CFStringRef name );
	
	long file_save_dialog( StrArg msg, StrArg name, HFS_Proc f, int ext = 0 );
	long file_save_dialog( StrArg msg, StrArg name, FSS_Proc f, int ext = 0 );
	
	long file_save_dialog( OSType type, OSType creator, FSRefName_callback f );
	
}
}

#endif
