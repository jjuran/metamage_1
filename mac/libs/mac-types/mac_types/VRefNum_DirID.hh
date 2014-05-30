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
	
	inline
	bool operator==( const VRefNum_DirID& a, const VRefNum_DirID& b )
	{
		return    a.vRefNum == b.vRefNum
		       && a.dirID   == b.dirID;
	}
	
	inline
	bool operator!=( const VRefNum_DirID& a, const VRefNum_DirID& b )
	{
		return !( a == b );
	}
	
}
}

#endif
