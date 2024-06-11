/*
	rsrc_fork.hh
	------------
*/

#ifndef RSRCFORK_HH
#define RSRCFORK_HH

// iota
#include "iota/ints.hh"


typedef char** Handle;

struct rsrc_fork_header
{
	iota::uint32_t  offset_to_data;
	iota::uint32_t  offset_to_map;
	iota::uint32_t  length_of_data;
	iota::uint32_t  length_of_map;
};

struct rsrc_fork_system_reserved
{
	iota::uint8_t bytes[ 112 ];
};

struct rsrc_fork_application_data
{
	iota::uint8_t bytes[ 128 ];
};

struct rsrc_fork_superheader : rsrc_fork_header
{
	rsrc_fork_system_reserved   reserved;
	rsrc_fork_application_data  app_data;
};

struct rsrc_data_element
{
	iota::uint32_t  length;
	iota::uint8_t   bytes[ 0x01000000 ];
};

struct type_header
{
	iota::uint32_t  type;     // a ResType
	iota::uint16_t  count_1;
	iota::uint16_t  offset;
};

struct rsrc_header
{
	iota::int16_t   id;
	iota::uint16_t  name_offset;
	iota::uint8_t   attrs;
	iota::uint8_t   offset_high_byte;
	iota::uint16_t  offset_low_word;
	Handle          handle;
};

struct type_list
{
	iota::uint16_t  count_1;
	type_header     list[ 65536 ];
};

struct rsrc_map_header
{
	rsrc_fork_header  fork_header;
	Handle            next_map;
	iota::int16_t     refnum;
	iota::uint16_t    attrs;
	iota::uint16_t    offset_to_types;
	iota::uint16_t    offset_to_names;
};

#endif
