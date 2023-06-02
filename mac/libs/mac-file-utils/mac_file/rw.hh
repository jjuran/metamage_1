/*
	rw.hh
	-----
*/

#ifndef MACFILE_RW_HH
#define MACFILE_RW_HH


namespace mac  {
namespace file {
	
	enum
	{
		fsAtMark    = 0,
		fsFromStart = 1,
		fsFromLEOF  = 2,
	};
	
	long  read ( short refnum, void*,       long n, short mode, long offset );
	short write( short refnum, const void*, long n, short mode, long offset );
	
	inline
	long read( short refnum, void* buffer, long n, long offset )
	{
		return read( refnum, buffer, n, fsFromStart, offset );
	}
	
	inline
	short write( short refnum, const void* buffer, long n, long offset )
	{
		return write( refnum, buffer, n, fsFromStart, offset );
	}
	
	inline
	long read( short refnum, void* buffer, long n )
	{
		return read( refnum, buffer, n, fsAtMark, 0 );
	}
	
	inline
	short write( short refnum, const void* buffer, long n )
	{
		return write( refnum, buffer, n, fsAtMark, 0 );
	}
	
	inline
	short append( short refnum, const void* buffer, long n )
	{
		return write( refnum, buffer, n, fsFromLEOF, 0 );
	}
	
	inline
	short read_all( short refnum, void* buffer, long n, long offset = 0 )
	{
		enum { eofErr = -39 };
		
		long n_read = read( refnum, buffer, n, fsFromStart, offset );
		
		if ( n_read < 0 )
		{
			return n_read;
		}
		
		if ( n_read < n )
		{
			return eofErr;
		}
		
		return 0;
	}
	
}
}

#endif
