/*
	UnitTable.hh
	------------
*/

#ifndef UNITTABLE_HH
#define UNITTABLE_HH

struct DCtlEntry;
struct DRVRHeader;

typedef short OSErr;
typedef char* Ptr;

DRVRHeader* get_driver( const DCtlEntry* dce );

short find_unit_entry_by_name( const unsigned char* name );

OSErr DrvrInstall_patch( DRVRHeader* d : __A0, short refnum : __D0 );

short install( DRVRHeader* d, short refnum );
short install( DRVRHeader* d );

#endif
