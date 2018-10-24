/*
	rsrc_fork.hh
	------------
*/

#ifndef RSRCFORK_HH
#define RSRCFORK_HH

// Standard C
#include <stdint.h>


struct rsrc_fork_header
{
	uint32_t  offset_to_data;
	uint32_t  offset_to_map;
	uint32_t  length_of_data;
	uint32_t  length_of_map;
};

struct rsrc_fork_system_reserved
{
	uint8_t bytes[ 112 ];
};

struct rsrc_fork_application_data
{
	uint8_t bytes[ 128 ];
};

struct rsrc_fork_superheader : rsrc_fork_header
{
	rsrc_fork_system_reserved   reserved;
	rsrc_fork_application_data  app_data;
};

struct rsrc_data_element
{
	uint32_t  length;
	uint8_t   bytes[ 0x01000000 ];
};

struct type_header
{
	uint32_t  type;     // a ResType
	uint16_t  count_1;
	uint16_t  offset;
};

struct rsrc_header
{
	int16_t   id;
	uint16_t  name_offset;
	uint8_t   attrs;
	uint8_t   offset_high_byte;
	uint16_t  offset_low_word;
	char**    handle;
};

struct type_list
{
	uint16_t     count_1;
	type_header  list[ 65536 ];
};

struct rsrc_map_header
{
	rsrc_fork_header  fork_header;
	char**            next_map;
	int16_t           refnum;
	uint16_t          attrs;
	uint16_t          offset_to_types;
	uint16_t          offset_to_names;
};

#endif
