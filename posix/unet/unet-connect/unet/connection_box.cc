/*
	connection_box.cc
	-----------------
*/

#include "unet/connection_box.hh"

// iota
#include "iota/swap.hh"

// poseven
#include "poseven/types/errno_t.hh"

// unet-connect
#include "unet/connection.hh"


namespace unet
{
	
	namespace p7 = poseven;
	
	
	connection_box::~connection_box()
	{
		if ( its_connection )
		{
			intrusive_ptr_release( its_connection );
		}
	}
	
	connection_box::connection_box( const connection_box& that )
	:
		its_connection( that.its_connection )
	{
		if ( its_connection )
		{
			intrusive_ptr_add_ref( its_connection );
		}
	}
	
	connection_box& connection_box::operator=( const connection_box& that )
	{
		if ( that.its_connection )
		{
			intrusive_ptr_add_ref( that.its_connection );
		}
		
		if ( its_connection )
		{
			intrusive_ptr_release( its_connection );
		}
		
		its_connection = that.its_connection;
		
		return *this;
	}
	
	void connection_box::swap( connection_box& that )
	{
		using iota::swap;
		
		swap( its_connection, that.its_connection );
	}
	
	connection_box::connection_box( const char* argv[], int in, int out )
	:
		its_connection( new connection( argv, in, out ) )
	{
		intrusive_ptr_add_ref( its_connection );
	}
	
	void connection_box::check() const
	{
		if ( its_connection == 0 )  // NULL
		{
			p7::throw_errno( ENOTCONN );
		}
	}
	
	int connection_box::get_input() const
	{
		return deref().get_input();
	}
	
	int connection_box::get_output() const
	{
		return deref().get_output();
	}
	
}
