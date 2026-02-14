/*
	vinfo.cc
	--------
	
	To run this in Advanced Mac Substitute:
	
		cd ~/src/metamage_1
		
		bin/rbuild -B46 vinfo
		
		cd ../ams-68k-bin
		
		graft \
			--fd 20  freemountd      -qu --root ~/var/tmp/ //  \
			--fd 21  freemountd --rw -qu --root ~/var/tmp/ //  \
			--fd 22  freemountd --rw -qu --root ~/var/tmp/ //  \
			xv68k -St                                          \
				-mlib/ams-{core,io}                                          \
				-m [ lib/ams-fs --mntfs-fd=20 --docfs-fd=21 --prefs-fd=22 ]  \
				~/var/build/68k-a4-blue-dbg/bin/vinfo/vinfo
	
*/

// Mac OS
#ifndef __APPLE__
#ifndef __FILES__
#include <Files.h>
#endif
#endif

// Standard C
#include <stdio.h>


static ParamBlockRec pb;

static Str27 bootstrap   = "\p" "Bootstrap";
static Str27 documents   = "\p" "Documents";
static Str27 preferences = "\p" "Preferences";

static StringPtr vol_names[] =
{
	bootstrap,
	documents,
	preferences,
};

static
void list_vols_by_name()
{
	OSErr err;
	
	StringPtr* it = vol_names;
	
	VolumeParam& vol = pb.volumeParam;
	
	vol.ioVolIndex = -1;
	
	for ( int i = 0;  i < 3;  ++i )
	{
		StringPtr name = vol_names[ i ];
		
		vol.ioNamePtr = name;
		
		err = PBGetVInfoSync( &pb );
		
		if ( err == noErr )
		{
			name[ 1 + name[ 0 ] ] = '\0';
			
			printf( "%s: %d\n", (const char*) name + 1, vol.ioVRefNum );
		}
	}
}

int main( int argc, char** argv )
{
	list_vols_by_name();
	
	return 0;
}
