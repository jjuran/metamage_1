/*
	object_file.hh
	--------------
	
	Copyright 2008-2010, Joshua Juran
*/

#ifndef METROWERKS_OBJECTFILE_HH
#define METROWERKS_OBJECTFILE_HH


namespace metrowerks
{
	
	typedef unsigned long uint32_t;
	
	
	enum magic_number_t
	{
		object_file_magic   = 'MWOB',
		runtime_block_magic = 0xfeedbead
	};
	
	enum cpu_architecture
	{
		cpu_unknown = 0,
		
		cpu_m68k    = 'M68K',
		cpu_powerpc = 'PPC '
	};
	
	enum runtime_architecture
	{
		runtime_native = 0,
		runtime_cfm68k = 1
	};
	
	
	struct object_file_header
	{
		magic_number_t    magic_number;
		cpu_architecture  cpu_arch;
		
		uint32_t _8, _12, _16, _20, _24, _28, _32, _36;
		
		uint32_t object_offset;
		uint32_t object_length;
	};
	
	struct runtime_block
	{
		magic_number_t        magic_number;
		runtime_architecture  runtime_arch;
	};
	
	struct object_header
	{
		magic_number_t magic_number;
		
		uint32_t _4, _8, _12, _16;
		
		uint32_t sym_offset;
		uint32_t sym_length;
	};
	
}

#endif

