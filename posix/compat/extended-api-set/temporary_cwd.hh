/*
	extended-api-set/temporary_cwd.hh
	---------------------------------
*/

#ifndef EXTENDEDAPISET_TEMPORARYCWD_HH
#define EXTENDEDAPISET_TEMPORARYCWD_HH


class _temporary_cwd
{
	private:
		const bool it_needs_chdir;
		const int  its_saved_cwd;
		int        it_has_failed;
		
	private:
		// non-copyable
		_temporary_cwd           ( const _temporary_cwd& );
		_temporary_cwd& operator=( const _temporary_cwd& );
	
	public:
		_temporary_cwd( int dirfd, const char* path );
		~_temporary_cwd();
		
		int failed() const  { return it_has_failed; }
};

#endif

