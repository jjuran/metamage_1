/*
	fd_pair.hh
	----------
*/

#ifndef POSEVEN_TYPES_FDPAIR_HH
#define POSEVEN_TYPES_FDPAIR_HH

// poseven
#ifndef POSEVEN_TYPES_FD_T_HH
#include "poseven/types/fd_t.hh"
#endif


namespace poseven
{
	
	class fd_pair
	{
		private:
			int its_fds[ 2 ];
			
			class xfer
			{
				private:
					fd_pair* ptr;
				
				public:
					xfer( fd_pair* that ) : ptr( that )
					{
					}
					
					fd_pair& get() const  { return *ptr; }
			};
		
		public:
			fd_pair()
			{
				its_fds[ 0 ] = -1;
				its_fds[ 1 ] = -1;
			}
			
			fd_pair( int fds[ 2 ] )
			{
				its_fds[ 0 ] = fds[ 0 ];
				its_fds[ 1 ] = fds[ 1 ];
			}
			
			fd_pair( fd_pair& that )
			{
				*this = that;
			}
			
			~fd_pair();
			
			operator xfer()  { return xfer( this ); }
			
			fd_pair( xfer x )
			{
				*this = x.get();
			}
			
			fd_pair& operator=( fd_pair& that )
			{
				its_fds[ 0 ] = that.its_fds[ 0 ];
				its_fds[ 1 ] = that.its_fds[ 1 ];
				
				that.release();
				
				return *this;
			}
			
			fd_t operator[]( int i ) const
			{
				return fd_t( its_fds[ i ] );
			}
			
			void release()
			{
				its_fds[ 0 ] = -1;
				its_fds[ 1 ] = -1;
			}
	};
	
	inline bool operator==( const fd_pair& a, const fd_pair& b )
	{
		return +   a[ 0 ] == b[ 0 ]
		       &&  a[ 1 ] == b[ 1 ];
	}
	
	inline bool operator!=( const fd_pair& a, const fd_pair& b )
	{
		return !( a == b );
	}
	
}

#endif
