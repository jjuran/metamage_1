/*
	buserror.c
	----------
*/


static
char bus_error_test()
{
	const char* unmapped_address = (const char*) 0x68f168f1;
	
	return *unmapped_address;
}

int main()
{
	bus_error_test();
	
	return 0;
}
