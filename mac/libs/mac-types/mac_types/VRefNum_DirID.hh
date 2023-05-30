/*
	VRefNum_DirID.hh
	----------------
*/

#ifndef MACTYPES_VREFNUMDIRID_HH
#define MACTYPES_VREFNUMDIRID_HH


namespace mac   {
namespace types {
	
#if ! __LP64__
typedef long SInt32;
#else
typedef int SInt32;
#endif
	
#pragma options align=packed
	
	struct VRefNum_DirID
	{
		short   vRefNum;
		SInt32  dirID;
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
	
	inline
	bool is_error( const VRefNum_DirID& dir )
	{
		return dir.vRefNum == 0;
	}
	
	inline
	long error( const VRefNum_DirID& dir )
	{
		return dir.dirID;
	}
	
}
}

#endif
