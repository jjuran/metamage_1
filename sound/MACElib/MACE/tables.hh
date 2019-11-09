/*
	tables.hh
	---------
*/

#ifndef MACE_TABLES_HH
#define MACE_TABLES_HH


namespace MACE
{
	
	typedef short int16_t;
	
	struct metatable
	{
		 const int16_t* one;
		 const int16_t* two;
		 
		 int stride;
	};
	
	extern const int16_t table_1[];
	extern const int16_t table_3[];
	
	extern const int16_t table_2[][ 4 ];
	extern const int16_t table_4[][ 2 ];
	
	extern const metatable metatables[];
	
}  // namespace MACE

#endif
