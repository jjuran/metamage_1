/*
	releasing.hh
	------------
*/

#ifndef RELEASING_HH
#define RELEASING_HH

// Mac OS X
#import <Foundation/Foundation.h>

// iota
#include "iota/class.hh"


class releasing
{
	NON_COPYABLE( releasing )
	
	private:
		id _object;
	
	public:
		releasing( id object ) : _object( object )
		{
		}
		
		~releasing()
		{
			[_object release];
		}
		
		operator id() const  { return _object; }
};

#endif
