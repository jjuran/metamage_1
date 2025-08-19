/*
	disc-id.hh
	----------
*/

#ifndef DISCID_DISCID_HH
#define DISCID_DISCID_HH


namespace disc_id
{
	
	typedef unsigned short start_t;
	
#if __LP64__
	
	typedef unsigned int  hash_t;
	
#else
	
	typedef unsigned long hash_t;
	
#endif
	
	hash_t hash( int n, const start_t* starts, start_t end );
	
}

#endif
