/*
	dir_contents_box.cc
	-------------------
*/

#include "vfs/dir_contents_box.hh"

// Debug
#include "debug/boost_assert.hh"

// Boost
#include <boost/intrusive_ptr.hpp>

// vfs
#include "vfs/dir_contents.hh"


namespace vfs
{
	
	dir_contents_box::dir_contents_box( const dir_contents_box& that )
	:
		its_contents( that.its_contents )
	{
		if ( its_contents )
		{
			intrusive_ptr_add_ref( its_contents );
		}
	}
	
	dir_contents_box& dir_contents_box::operator=( const dir_contents_box& that )
	{
		if ( that.its_contents )
		{
			intrusive_ptr_add_ref( that.its_contents );
		}
		
		if ( its_contents )
		{
			intrusive_ptr_release( its_contents );
		}
		
		its_contents = that.its_contents;
		
		return *this;
	}
	
	dir_contents_box::~dir_contents_box()
	{
		if ( its_contents )
		{
			intrusive_ptr_release( its_contents );
		}
	}
	
	void dir_contents_box::swap( dir_contents_box& that )
	{
		dir_contents* temp = its_contents;
		
		its_contents = that.its_contents;
		
		that.its_contents = temp;
	}
	
	dir_contents* dir_contents_box::get_new()
	{
		if ( its_contents == NULL )
		{
			its_contents = new dir_contents();
			
			intrusive_ptr_add_ref( its_contents );
		}
		
		return its_contents;
	}
	
}

