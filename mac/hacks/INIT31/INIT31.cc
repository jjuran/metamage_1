/*
	INIT31.cc
	---------
*/

// Mac OS
#ifndef __RESOURCES__
#include <Resources.h>
#endif


#pragma exceptions off


QHdr VCBQHdr : 0x0356;


static inline
bool has_INITs( OSType type )
{
	return type == 'INIT'  ||  type == 'RDEV'  ||  type == 'cdev';
}

static inline
bool has_INITs( const FInfo& info )
{
	return ! (info.fdFlags & kHasNoINITs)  &&  has_INITs( info.fdType );
}

static
asm
void call_INIT( Handle code : __A0 )
{
	LINK     A6,#0
	MOVEM.L  D3-D7/A2-A6,-(SP)
	
	MOVEA.L  (A0),A1
	JSR      (A1)
	
	MOVEM.L  (SP)+,D3-D7/A2-A6
	UNLK     A6
	RTS
}

static
void run_INITs()
{
	short n_INITs = Count1Resources( 'INIT' );
	
	for ( short i = 1;  i <= n_INITs;  ++i )
	{
		if ( Handle h = Get1IndResource( 'INIT', i ) )
		{
			HLock( h );
			
			call_INIT( h );
		}
	}
}

static inline
short get_Bootstrap_vRefNum()
{
	VCB* vcb = (VCB*) VCBQHdr.qHead;
	
	for ( ;  vcb != NULL;  vcb = (VCB*) vcb->qLink )
	{
		if ( vcb->vcbSigWord == 'Ix' )
		{
			return vcb->vcbVRefNum;
		}
	}
	
	return 0;
}

static inline
bool get_next_file_info( ParamBlockRec& pb, int& i )
{
	pb.fileParam.ioFDirIndex = ++i;
	
	OSErr err = PBGetFInfoSync( &pb );
	
	return err == noErr;
}

int main()
{
	Str255 name;
	ParamBlockRec pb;
	
	pb.fileParam.ioVRefNum = get_Bootstrap_vRefNum();
	pb.fileParam.ioNamePtr = name;
	
	int i = 0;
	
	while ( get_next_file_info( pb, i ) )
	{
		if ( has_INITs( pb.fileParam.ioFlFndrInfo ) )
		{
			short refnum = OpenResFile( name );
			
			if ( refnum > 0 )
			{
				run_INITs();
				
				CloseResFile( refnum );
			}
		}
	}
	
	return 0;
}
