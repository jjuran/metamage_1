/*	=======
 *	true.cc
 *	=======
 */


#pragma export on

int main( void )
{
	return 0;
}

extern "C" int KMain( void* env );

int KMain( void* )
{
	return main();
}

#pragma export reset

