/*
	coprocess_box.cc
	----------------
*/

#include "poseven/extras/coprocess_box.hh"

// iota
#include "iota/swap.hh"

// poseven
#include "poseven/extras/coprocess.hh"


namespace poseven
{
	
	coprocess_box::~coprocess_box()
	{
		if ( its_coprocess )
		{
			intrusive_ptr_release( its_coprocess );
		}
	}
	
	coprocess_box::coprocess_box( const coprocess_box& that )
	:
		its_coprocess( that.its_coprocess )
	{
		if ( its_coprocess )
		{
			intrusive_ptr_add_ref( its_coprocess );
		}
	}
	
	coprocess_box& coprocess_box::operator=( const coprocess_box& that )
	{
		if ( that.its_coprocess )
		{
			intrusive_ptr_add_ref( that.its_coprocess );
		}
		
		if ( its_coprocess )
		{
			intrusive_ptr_release( its_coprocess );
		}
		
		its_coprocess = that.its_coprocess;
		
		return *this;
	}
	
	void coprocess_box::swap( coprocess_box& that )
	{
		using iota::swap;
		
		swap( its_coprocess, that.its_coprocess );
	}
	
	coprocess_box::coprocess_box( ::pid_t pid )
	{
		// Acquire pid on the stack in case new fails
		coprocess temp(( pid_t( pid ) ));
		
		its_coprocess = new coprocess( temp.move() );
		
		intrusive_ptr_add_ref( its_coprocess );
	}
	
}
