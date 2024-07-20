/*
	addrerror.c
	-----------
*/


static
int address_error_read_test()
{
	int x = 0;
	
	return *(short*) ((char*) &x + 1);
}

int main()
{
	address_error_read_test();
	
	return 0;
}
