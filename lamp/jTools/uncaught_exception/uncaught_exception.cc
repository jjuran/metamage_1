/*	=====================
 *	uncaught_exception.cc
 *	=====================
 */


#pragma export on

class FooBar {};

int main( int argc, char const *const argv[] )
{
	throw FooBar();
	
	return 0;  // Not reached
}

#pragma export reset

