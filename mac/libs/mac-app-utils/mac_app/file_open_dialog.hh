/*
	file_open_dialog.hh
	-------------------
*/

#ifndef MACAPP_FILEOPENDIALOG_HH
#define MACAPP_FILEOPENDIALOG_HH

struct FSRef;
struct FSSpec;

typedef unsigned char Byte;

namespace mac {
namespace app {
	
	typedef unsigned long OSType;
	
	typedef long (*HFS_callback)( short vRefNum, long dirID, const Byte* name );
	
	typedef long (*FSSpec_callback)( const FSSpec& file );
	typedef long (*FSRef_callback )( const FSRef&  file );
	
	long file_open_dialog( const OSType* types, int n, HFS_callback    proc );
	long file_open_dialog( const OSType* types, int n, FSSpec_callback proc );
	long file_open_dialog( const OSType* types, int n, FSRef_callback  proc );
	
}
}

#endif
