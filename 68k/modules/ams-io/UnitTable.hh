/*
	UnitTable.hh
	------------
*/

#ifndef UNITTABLE_HH
#define UNITTABLE_HH

struct DCtlEntry;
struct DRVRHeader;

DRVRHeader* get_driver( const DCtlEntry* dce );

short find_unit_entry_by_name( const unsigned char* name );

short install( DRVRHeader** d, short i );
short install( DRVRHeader** d );

#endif
