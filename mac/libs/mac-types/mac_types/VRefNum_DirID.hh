/*
	VRefNum_DirID.hh
	----------------
*/

#ifndef MACTYPES_VREFNUMDIRID_HH
#define MACTYPES_VREFNUMDIRID_HH


namespace mac   {
namespace types {
	
#pragma options align=mac68k
	
	struct VRefNum_DirID
	{
		short  vRefNum;
		long   dirID;
	};
	
#pragma options align=reset
	
}
}

#endif
