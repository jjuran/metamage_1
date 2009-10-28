/*
	fd_reader.hh
	------------
	
	Copyright 2009, Joshua Juran
*/


#ifndef POSEVEN_EXTRAS_FDREADER_HH
#define POSEVEN_EXTRAS_FDREADER_HH

// Standard C++
#include <functional>

// poseven
#include "poseven/extras/read_all.hh"


namespace poseven
{
	
	class fd_reader : public std::binary_function< char*, size_t, ssize_t >
	{
		private:
			fd_t its_fd;
		
		public:
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

