/*	===========
 *	buserror.cc
 *	===========
 */


#pragma exceptions off


static int BusErrorTest()
{
	const int* unmapped_address = (const int*) 0x68f168f1;
	
	return *unmapped_address;
}

int main()
{
	//Debugger();
	
	BusErrorTest();
	
	return 0;
}

