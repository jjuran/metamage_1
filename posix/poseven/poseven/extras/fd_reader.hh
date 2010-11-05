/*
	fd_reader.hh
	------------
	
	Copyright 2009, Joshua Juran
*/


#ifndef POSEVEN_EXTRAS_FDREADER_HH
#define POSEVEN_EXTRAS_FDREADER_HH

// poseven
#ifndef POSEVEN_EXTRAS_READALL_HH
#include "poseven/extras/read_all.hh"
#endif


namespace poseven
{
	
	class fd_reader
	{
		private:
			fd_t its_fd;
		
		public:
			typedef char*    first_argument_type;
			typedef size_t   second_argument_type;
			typedef ssize_t  result_type;
			
			fd_reader( fd_t fd ) : its_fd( fd )
			{
			}
			
			ssize_t operator()( char* buffer, size_t length ) const
			{
				return read_some( its_fd, buffer, length );
			}
	};
	
}

#endif

