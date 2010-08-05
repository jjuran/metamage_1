/*
	extended-api-set/dual_path_at.hh
	--------------------------------
*/

#ifndef EXTENDEDAPISET_DUALPATHAT_H
#define EXTENDEDAPISET_DUALPATHAT_H


typedef int (*dual_path_filing_op)( const char* oldpath, const char* newpath, int flags );

int _dual_path_at( dual_path_filing_op  op,
                   int                  olddirfd,
                   const char*          oldpath,
                   int                  newdirfd,
                   const char*          newpath,
                   int                  flags = 0 );

#endif

