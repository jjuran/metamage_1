/*	=====================
 *	uncaught_exception.cc
 *	=====================
 */


class FooBar {};

int main( int argc, char const *const argv[] )
{
	throw FooBar();
	
	return 0;  // Not reached
}
