/*
	d68k.cc
	-------
*/

// Standard C++
#include <vector>

// Standard C
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// more-libc
#include "more/string.h"

// iota
#include "iota/char_types.hh"
#include "iota/endian.hh"
#include "iota/strings.hh"

// command
#include "command/get_option.hh"

// gear
#include "gear/hexadecimal.hh"
#include "gear/inscribe_decimal.hh"

// plus
#include "plus/var_string.hh"

// poseven
#include "poseven/functions/dup2.hh"
#include "poseven/functions/open.hh"
#include "poseven/functions/read.hh"
#include "poseven/functions/write.hh"

// Orion
#include "Orion/Main.hh"

// d68k
#include "traps.hh"


/*
	A 68K disassembler.
	
	Line 0:  complete
	Line 1:  complete  (MOVE.B)
	Line 2:  complete  (MOVE.L)
	Line 3:  complete  (MOVE.W)
	Line 4:  complete
	Line 5:  complete  (ADDQ, SUBQ, DBcc)
	Line 6:  complete  (Bcc)
	Line 7:  complete  (MOVEQ)
	Line 8:  complete
	Line 9:  complete (SUB)
	Line A:  A-Traps
	Line B:  complete
	Line C:  complete
	Line D:  complete (ADD)
	Line E:  complete (shift/rotate)
	Line F:  F-Traps
*/


using namespace command::constants;

enum
{
	Option_quiet = 'q',
};

static command::option options[] =
{
	{ "quiet", Option_quiet },
	
	{ NULL }
};

static bool quiet = false;

static
char* const* get_options( char* const* argv )
{
	++argv;  // skip arg 0
	
	short opt;
	
	while ( (opt = command::get_option( &argv, options )) )
	{
		switch ( opt )
		{
			case Option_quiet:
				quiet = true;
				break;
			
			default:
				abort();
		}
	}
	
	return argv;
}

namespace tool
{
	
	namespace p7 = poseven;
	
	
	#define COMMENT  "    // "
	
	
	static inline int sign_extend_char( signed char x )
	{
		return x;
	}
	
	static void append_hex( plus::var_string& s, uint32_t x, int min_bytes )
	{
		const uint16_t min_digits = min_bytes * 2;
		
		const uint16_t magnitude = gear::hexadecimal_magnitude( x );
		
		const uint16_t even_magnitude = magnitude + (magnitude & 0x1 );
		
		const uint16_t n_bytes = std::max( even_magnitude, min_digits );
		
		s.resize( s.size() + n_bytes );
		
		char* buf = &*s.end() - n_bytes;
		
		gear::inscribe_n_hex_digits( buf, x, n_bytes );
	}
	
	static void append_signed_decimal( plus::var_string& s, int x )
	{
		char sign = '+';
		
		if ( x < 0 )
		{
			sign = '-';
			
			x = -x;
		}
		
		s += sign;
		
		s += gear::inscribe_decimal( x );
	}
	
	static inline const char* get_aTrap_name( uint16_t trap_word )
	{
		return get_trap_name( trap_word );
	}
	
	
	class end_of_file {};
	
	class illegal_instruction {};
	
	class illegal_operand {};
	
	
	static char size_codes[] =
	{
		'B',
		'W',
		'L'
	};
	
	static char sizes[] =
	{
		1,
		2,
		4
	};
	
	static const char* bit_ops[] =
	{
		"TST",
		"CHG",
		"CLR",
		"SET"
	};
	
	static const char* bfonly_ops[] =
	{
		"EXTU",
		"EXTS",
		"FFO",
		"INS"
	};
	
	static const char* bit_slide_ops[] =
	{
		"AS",
		"LS",
		"ROX",
		"RO"
	};
	
	static const char* condition_codes[] =
	{
		"T ",
		"F ",
		"HI",
		"LS",
		"CC",
		"CS",
		"NE",
		"EQ",
		"VC",
		"VS",
		"PL",
		"MI",
		"GE",
		"LT",
		"GT",
		"LE"
	};
	
	
	static bool globally_prefix_address;
	static bool globally_attach_target_comments;
	
	static uint32_t global_bytes_read = 0;
	
	static uint32_t global_pc = 0;
	
	static uint16_t global_last_op = 0;
	
	static uint32_t global_last_CMPI_operand = 0;
	
	static uint32_t global_last_branch_target = 0;
	static uint32_t global_last_pc_relative_target = 0;
	static uint32_t global_successor_of_last_exit = 0;
	
	static std::vector< uint32_t > global_branch_targets;
	
	static std::vector< uint32_t > global_entry_points;
	static std::vector< uint32_t > global_exit_points;
	
	
	static const uint16_t indexed_jump_code[] =
	{
		//0xd040,  // ADD.W    D0,D0
		
		//0x303b,  // MOVE.W   (6,PC,D0.W),D0
		//0x0006,
		
		0x4efb,  // JMP      (2,PC,D0.W)
		0x0002
	};
	
	static int indexed_jump_state = 0;
	
	static bool at_indexed_jump = false;
	
	
	static const uint16_t lswtch_code[] =
	{
		0x205f,
		0x2248,
		0xd2d8,
		0xb098,
		0x6c02,
		0x4ed1,
		0xb098,
		0x6f02,
		0x4ed1,
		0x3218,
		0xb098,
		0x6604,
		0xd0d0,
		0x4ed0,
		0x5448,
		0x51c9,
		0xfff4,
		0x4ed1
	};
	
	static int lswtch_state = 0;
	
	static uint32_t lswtch_offset = 0;
	
	
	static void add_branch_target( uint32_t address )
	{
		typedef std::vector< uint32_t >::iterator iterator;
		
		const iterator it = std::lower_bound( global_branch_targets.begin(),
		                                      global_branch_targets.end(),
		                                      address );
		
		const bool found = it != global_branch_targets.end()  &&  *it == address;
		
		if ( !found )
		{
			global_branch_targets.insert( it, address );
		}
	}
	
	static bool check_branch_target( uint32_t address )
	{
		typedef std::vector< uint32_t >::iterator iterator;
		
		const iterator it = std::lower_bound( global_branch_targets.begin(),
		                                      global_branch_targets.end(),
		                                      address );
		
		const bool found = it != global_branch_targets.end()  &&  *it == address;
		
		global_branch_targets.erase( global_branch_targets.begin(), it );
		
		return found;
	}
	
	static void add_exit_point( uint32_t address )
	{
		global_exit_points.push_back( address );
	}
	
	static void add_entry_point( uint32_t address )
	{
		// Add an entry point unless
		// * it already exists
		// * it lies between an entry point and an exit point (or eof)
		
		typedef std::vector< uint32_t >::iterator iterator;
		
		// Search for an exit point equal to or prior to the address
		
		const iterator it = std::upper_bound( global_exit_points.begin(),
		                                      global_exit_points.end(),
		                                      address );
		
		// it now points to an exit point greater than address, or the end.
		// it's either at the beginning or preceded by an exit point less or equal.
		
		if ( it == global_exit_points.begin() )
		{
			// no prior exit point, so entry point is superfluous
			
			return;
		}
		
		const uint32_t previous_exit = *it;
		
		const iterator jt = std::lower_bound( global_entry_points.begin(),
		                                      global_entry_points.end(),
		                                      previous_exit );
		
		// jt now points to an entry point >= the exit point, or the end
		// We must add an entry point if (a) none follows the exit point, or
		// (b) the next entry point exceeds the address.
		
		if ( jt == global_entry_points.end()  ||  *jt > address )
		{
			global_entry_points.push_back( address );
		}
	}
	
	static uint16_t read_word( bool peeking = false )
	{
		const size_t buffer_size = 4096;
		
		static char buffer[ buffer_size ];
		
		static char* end = NULL;
		static char* p   = NULL;
		
		if ( p == end )
		{
			const ssize_t n_read = p7::read( p7::stdin_fileno, buffer, buffer_size );
			
			if ( n_read == 0 )
			{
				throw end_of_file();
			}
			
			p   = buffer;
			end = buffer + n_read;
		}
		
		if ( end - p == 1 )
		{
			// Odd byte
			
			const size_t got = p7::read( p7::stdin_fileno, end, sizeof end[0] );
			
			if ( got )
			{
				++end;
				
				// We have a full word
			}
			else
			{
				// or we're at end of file
				p7::write( p7::stderr_fileno, STR_LEN( "d68k: Warning: dropping odd last byte.\n" ) );
				
				throw end_of_file();
			}
		}
		
		const uint16_t result = iota::u16_from_big( *(uint16_t *) p );
		
		if ( !peeking )
		{
			p += sizeof (uint16_t);
			
			global_bytes_read += sizeof (uint16_t);
			
			if ( !at_indexed_jump )
			{
				if ( result == indexed_jump_code[ indexed_jump_state ] )
				{
					if ( ++indexed_jump_state == sizeof indexed_jump_code / sizeof indexed_jump_code[0] )
					{
						at_indexed_jump = true;
						
						indexed_jump_state = 0;
					}
				}
				else
				{
					indexed_jump_state = 0;
				}
			}
			
			if ( lswtch_offset == 0 )
			{
				if ( const bool match = result == lswtch_code[ lswtch_state ] )
				{
					if ( ++lswtch_state == sizeof lswtch_code / sizeof lswtch_code[0] )
					{
						lswtch_offset = global_bytes_read - sizeof lswtch_code;
					}
				}
				else
				{
					lswtch_state = 0;
				}
			}
		}
		
		return result;
	}
	
	static inline uint16_t peek_word()
	{
		return read_word( true );
	}
	
	static inline short read_word_signed()
	{
		return read_word();
	}
	
	static uint32_t read_long()
	{
		const uint16_t high = read_word();
		const uint16_t low  = read_word();
		
		return uint32_t( high ) << 16 | low;
	}
	
	
	static void set_register_name( char* name, short mode, short n )
	{
		name[0] = mode == 0 ? 'D' : 'A';
		name[1] = '0' + n;
	}
	
	static int read_extended_displacement( uint16_t size_code )
	{
		switch ( size_code )
		{
			case 1:
				return 0;
			
			case 2:
				return read_word_signed();
			
			case 3:
				return read_long();
		}
		
		// Error if reached
		return 0;
	}
	
	static uint32_t global_last_absolute_addr_from_ea  = 0;
	static uint32_t global_last_immediate_data_from_ea = 0;
	
	static plus::string read_ea( short mode_reg, short immediate_size )
	{
		global_last_absolute_addr_from_ea = 0;
		
		const short mode = mode_reg >> 3;
		
		const short reg = mode_reg & 0x7;
		
		char reg_name[3] = "PC";
		
		if ( mode != 7 )
		{
			set_register_name( reg_name, mode, reg );
		}
		
		if ( mode <= 1 )
		{
			// Data Register Direct
			// Address Register Direct
			
			return reg_name;
		}
		
		plus::var_string result;
		
		if ( mode <= 4 )
		{
			// Address Register Indirect
			// Address Register Indirect with Postincrement
			// Address Register Indirect with Predecrement
			
			if ( mode == 4 )
			{
				result += "-";
			}
			
			result += "(";
			result += reg_name;
			result += ")";
			
			if ( mode == 3 )
			{
				result += "+";
			}
			
			return result;
		}
		
		uint16_t extension = read_word();
		
		if ( mode == 5  ||  (mode == 7  &&  reg == 2) )
		{
			// Address Register Indirect with Displacement
			// Program Counter Indirect with Displacement
			
			const short displacement = extension;
			
			if ( const bool pc_relative = mode == 7  &&  immediate_size == 0 )
			{
				result += '*';
				
				append_signed_decimal( result, displacement + 2 );
				
				global_last_pc_relative_target = global_pc + displacement;
			}
			else
			{
				result += "(";
				
				if ( displacement )
				{
					result += gear::inscribe_decimal( displacement );
					
					result += ",";
				}
				
				result += reg_name;
				result += ")";
			}
		}
		else if ( mode == 6  ||  (mode == 7  &&  reg == 3) )
		{
			// Address Register Indirect with Index (8-bit Displacement)
			// Address Register Indirect with Index (Base Displacement)
			// Memory Indirect Postindexed
			// Memory Indirect Preindexed
			
			// Program Counter Indirect with Index (8-bit Displacement)
			// Program Counter Indirect with Index (Base Displacement)
			// Program Counter Memory Indirect Postindexed
			// Program Counter Memory Indirect Preindexed
			
			const short index_reg = extension >> 12;
			
			char index_reg_name[3] = "Rn";
			
			set_register_name( index_reg_name, index_reg & 0x8, index_reg & 0x7 );
			
			const bool full_format = extension & 0x0100;
			
			const bool base_suppress  = full_format * extension & 0x80;
			const bool index_suppress = full_format * extension & 0x40;
			
			const int base_displacement = full_format ? read_extended_displacement( extension >> 4 & 0x3 )
			                                          : sign_extend_char( extension & 0xff );
			
			const int iis = full_format * extension & 0x7;
			
			const bool memory_indirect = iis != 0;
			
			result += "(";
			
			if ( memory_indirect )
			{
				result += "[";
			}
			
			bool needs_comma = false;
			
			if ( base_displacement )
			{
				result += gear::inscribe_decimal( base_displacement );
				
				needs_comma = true;
			}
			
			if ( !base_suppress )
			{
				if ( needs_comma )
				{
					result += ",";
				}
				
				needs_comma = true;
				
				result += reg_name;
			}
			
			const bool postindexed = iis & 0x4;
			
			if ( postindexed )
			{
				if ( !base_displacement  &&  base_suppress )
				{
					result += "0";
					
					needs_comma = true;
				}
				
				result += "]";
			}
			
			if ( !index_suppress )
			{
				if ( needs_comma )
				{
					result += ",";
				}
				
				const int index_width = extension >> 11 & 0x1;
				
				const char *widths = "WL";
				
				const int scale_bits = extension >> 9 & 0x3;
				
				result += index_reg_name;
				result += ".";
				result += widths[ index_width ];
				
				if ( scale_bits )
				{
					const int scale = 1 << scale_bits;
					
					result += "*";
					result += '0' + scale;
				}
				
				needs_comma = true;
			}
			
			if ( memory_indirect  &&  index_suppress  &&  !postindexed )
			{
				result += "]";
			}
			
			const int outer_displacement = full_format ? read_extended_displacement( iis & 0x3 )
			                                           : 0;
			
			if ( outer_displacement )
			{
				result += "," "0x";
				append_hex( result, outer_displacement, 2 );
			}
			
			result += ")";
		}
		else if ( mode == 7 )
		{
			// Absolute Short Address
			// Absolute Long Address
			// Immediate
			
			switch ( reg )
			{
				case 0:
				case 1:
					immediate_size = (reg + 1) << 1;
					
					break;
				
				case 4:
					result += "#";
					
					break;
				
				default:
					throw illegal_operand();
					break;
			}
			
			uint32_t& marker = reg <= 1 ? global_last_absolute_addr_from_ea
			                            : global_last_immediate_data_from_ea;
			
			result += "0x";
			
			const uint32_t data = immediate_size == 1 ? extension & 0xff
			                    : immediate_size == 2 ? extension
			                    :                       extension << 16 | read_word();
			
			marker = data;
			
			append_hex( result, data, immediate_size );
		}
		
		return result;
	}
	
	
	typedef void (*decoder)( uint16_t op );
	
	static void decode_default( uint16_t op )
	{
		printf( "%#.4x\n", op );
	}
	
	
	static const char* immediate_ops[] =
	{
		"ORI",
		"ANDI",
		"SUBI",
		"ADDI",
		NULL,
		"EORI",
		"CMPI",
		NULL
	};
	
#pragma mark -
#pragma mark ** Line 0 **
	
	static void decode_compare( uint16_t op )
	{
		const bool compare_and_swap = op & 0x0800;
		
		const short size_index = (op >> 9 & 0x3) - compare_and_swap;
		
		if ( uint16_t( size_index & 0x3 ) == 0x3 )
		{
			throw illegal_instruction();
		}
		
		const short mode_reg = op & 0x3f;
		
		const bool is_cas2 = mode_reg == 0x3c;
		
		const uint16_t ext1 =           read_word();
		const uint16_t ext2 = is_cas2 ? read_word() : 0;
		
		const bool is_chk2 = ext1 & 0x0800;
		
		const char* op_name = compare_and_swap ? is_cas2 ? "CAS2"
		                                                 : "CAS "
		                                       : is_chk2 ? "CHK2"
		                                                 : "CMP2";
		
		printf( "%s     ...\n", op_name );
	}
	
	static void decode_Immediate( uint16_t op )
	{
		const short size_index = op >> 6 & 0x3;
		
		if ( size_index == 3 )
		{
			decode_compare( op );
			
			return;
		}
		
		const char* format = "%s%s%s#%#x,%s" "\n";
		
		const char* name = immediate_ops[ op >> 9 & 0x7 ];
		
		if ( op & 0x0100  ||  name == NULL )
		{
			throw illegal_instruction();
		}
		
		const char* space = "      ";
		
		if ( name[ STRLEN( "AND" ) ] == 'I' )
		{
			++space;
		}
		
		const short immediate_size = sizes[ size_index ];
		
		const short mode_reg = op & 0x3f;
		
		uint32_t immediate_data = read_word();
		
		if ( mode_reg == 0x3c )
		{
			printf( format, name, "", space, immediate_data, size_index ? "SR" : "CCR" );
		}
		else
		{
			const char qualifier[] = { '.', size_codes[ size_index ], '\0' };
			
			space += 2;
			
			if ( size_index == 2 )
			{
				immediate_data = immediate_data << 16 | read_word();
			}
			
			if ( (op >> 9 & 0x7) == 6 )
			{
				// needed for index jumps
				global_last_CMPI_operand = immediate_data;
			}
			
			const plus::string ea = read_ea( mode_reg, immediate_size );
			
			printf( format, name, qualifier, space, immediate_data, ea.c_str() );
		}
	}
	
	static void decode_Bit_op( uint16_t op, bool dynamic )
	{
		char dynamic_format[] = "Bfoo     D%d,%s"  "\n";
		char static_format [] = "Bfoo     #%#x,%s" "\n";
		
		char* format = dynamic ? dynamic_format
		                       : static_format;
		
		const char* name = bit_ops[ op >> 6 & 0x3 ];
		
		const size_t name_len = STRLEN( "foo" );
		
		mempcpy( format + STRLEN( "B" ), name, name_len );
		
		const short mode_reg = op & 0x3f;
		
		const int data = dynamic ? op >> 9       // data register
		                         : read_word();  // immediate data
		
		const short immediate_size = 1;
		
		const plus::string ea = read_ea( mode_reg, immediate_size );
		
		printf( format, data, ea.c_str() );
	}
	
#pragma mark -
#pragma mark ** Line 1-3 **
	
	static void decode_MOVE( uint16_t op, short size_index )
	{
		const short immediate_size = sizes[ size_index ];
		
		const short source_mode_reg = op & 0x3f;
		
		const short dest_mode_reg = (op >> 3 & 0x38) | (op >> 9 & 0x07);
		
		plus::var_string comment;
		
		if ( immediate_size == 2  &&  dest_mode_reg == 0  &&  source_mode_reg == 0x3c )
		{
			const uint16_t data = peek_word();
			
			if ( (data & 0xf000) == 0xa000 )
			{
				if ( const char* name = get_aTrap_name( data ) )
				{
					comment = COMMENT;
					
					comment += name;
				}
			}
		}
		
		const plus::string source = read_ea( source_mode_reg, immediate_size );
		
		if ( immediate_size == 4  &&  source_mode_reg == 0x3c )
		{
			const uint32_t data = global_last_immediate_data_from_ea;
			
			/*
				Since we're determining if this value is human-readable or
				not, it's not a serious problem to use the locale-dependent
				isprint().  However, it's better if output is deterministic,
				so use a pure function.
			*/
			
			if (     iota::is_print( data >> 24        )
			     &&  iota::is_print( data >> 16 & 0xff )
			     &&                ( data >>  8 & 0xff ) >= ' '
			     &&                ( data       & 0xff ) >= ' ' )
			{
				comment = COMMENT;
				
				const char osType[] =
				{
					'\'',
					char( data >> 24 ),
					char( data >> 16 ),
					char( data >>  8 ),
					char( data       ),
					'\'',
					'\0'
				};
				
				comment += osType;
			}
		}
		
		const plus::string dest = read_ea( dest_mode_reg, immediate_size );
		
		const bool address = (dest_mode_reg >> 3) == 1;
		
		const char* format = address ? "MOVEA.%c  %s,%s%s" "\n"
		                             : "MOVE.%c   %s,%s%s" "\n";
		
		printf( format, size_codes[ size_index ], source.c_str(),
		                                          dest.c_str(),
		                                          comment.c_str() );
	}
	
#pragma mark -
#pragma mark ** Line 4 **
	
	static char const* const unary_ops[] =
	{
		"NEGX",
		"CLR",
		"NEG",
		"NOT",
		"",
		"TST",
		"",
		""
	};
	
	static void decode_unary( uint16_t op )
	{
		const short size_index = op >> 6 & 0x3;
		
		if ( size_index == 3 )
		{
			throw illegal_instruction();
		}
		
		char format[] = "%s.%c    %s" "\n";
		
		const char* name = unary_ops[ op >> 9 & 0x7 ];
		
		if ( op & 0x0100  ||  *name == '\0' )
		{
			throw illegal_instruction();
		}
		
		const short immediate_size = sizes[ size_index ];
		
		const plus::string ea = read_ea( op & 0x3f, immediate_size );
		
		printf( format, name, size_codes[ size_index ], ea.c_str() );
	}
	
	static const char* move_ccr_sr[] =
	{
		"MOVE     SR,%s"  "\n",
		"MOVE     CCR,%s" "\n",
		"MOVE     %s,CCR" "\n",
		"MOVE     %s,SR"  "\n"
	};
	
	static void decode_4_line_special( uint16_t op )
	{
		if ( op == 0x4afc )
		{
			printf( "%s" "\n", "ILLEGAL" );
			
			return;
		}
		
		const bool tas = (op & 0x0f00) == 0x0a00;
		
		const uint16_t immediate_size = tas ? 1 : 2;
		
		const plus::string ea = read_ea( op & 0x3f, immediate_size );
		
		const char* format = tas ? "TAS.B    %s" "\n"
		                         : move_ccr_sr[ op >> 9 & 0x3 ];
		
		printf( format, ea.c_str() );
	}
	
	static bool jump_breaks_routine( uint16_t mode_reg )
	{
		// Whether a jump marks the end of routine:
		// * only JMP breaks a routine, not JSR
		// * source == 0x3b is used for machine-specific branching, not a break
		// * a jump followed by the last branch target's entry point is not a break
		
		if ( mode_reg == 0x3b )
		{
			return false;
		}
		
		const bool resumes =  global_bytes_read == global_last_branch_target;
		
		return !resumes;
	}
	
	static void print_comment( uint32_t pc_relative_target )
	{
		printf( COMMENT "$%.6x", pc_relative_target );
	}
	
	static void decode_jump_table()
	{
		printf( "// indexed jump table\n" );
		
		const uint32_t jump_table = global_bytes_read;
		
		int n_jumps = global_last_CMPI_operand;
		
		while ( n_jumps-- >= 0 )
		{
			printf( "// goto $%.6x\n", jump_table + read_word() );
		}
	}
	
	static void decode_switch_table()
	{
		printf( "// __lswtch__ table\n" );
		
		const uint32_t table_start = global_bytes_read;
		
		const uint32_t default_case = table_start + read_word();
		
		printf( "// default:  goto $%.6x\n", default_case );
		
		const uint32_t min = read_long();
		
		printf( "// min: %#x, %d\n", min, min );
		
		const uint32_t max = read_long();
		
		printf( "// max: %#x, %d\n", max, max );
		
		int n = read_word();
		
		while ( n-- >= 0 )
		{
			const uint32_t value = read_long();
			
			uint32_t target = global_bytes_read;
			
			const uint16_t offset = read_word();
			
			target += offset;
			
			printf( "// case %#x, %d:  goto $%.6x\n", value, value, target );
		}
	}
	
	static void decode_Jump( uint16_t op )
	{
		const uint16_t source = op & 0x3f;
		
		const bool jump = op & 0x0040;
		
		const char* format = "%s      %s";
		
		const char* op_name = jump ? "JMP" : "JSR";
		
		const plus::string ea = read_ea( source, 0 );
		
		printf( format, op_name, ea.c_str() );
		
		if ( globally_attach_target_comments  &&  source == 0x3a )
		{
			print_comment( global_last_pc_relative_target );
		}
		
		const char* newlines = "\n\n";
		
		const bool breaks = jump  &&  jump_breaks_routine( source );
		
		if ( !breaks )
		{
			++newlines;
		}
		
		printf( "%s", newlines );
		
		if ( jump )
		{
			if ( at_indexed_jump )
			{
				const bool fpu_selector = global_last_op == 0xc0fc;
				
				if ( !fpu_selector )
				{
					decode_jump_table();
				}
				
				at_indexed_jump = false;
			}
			else
			{
				global_successor_of_last_exit = global_bytes_read;
			}
		}
		else if ( lswtch_offset  &&  global_last_absolute_addr_from_ea == lswtch_offset )
		{
			decode_switch_table();
		}
	}
	
	static const char *const swap_ext[] =
	{
		"",
		"SWAP ",
		"EXT.W",
		"EXT.L"
	};
	
	static const char *const ops_4e7x[] =
	{
		"RESET" "\n",
		"NOP" "\n",
		"STOP     #%#x" "\n",
		"RTE" "\n",
		"RTD      #%d"  "\n",
		"RTS" "\n",
		"TRAPV" "\n",
		"RTR" "\n"
	};
	
	static char* get_register_sequence( char* buffer, uint16_t mask, char c )
	{
		char* p = buffer;
		
		int first_in_run = -1;
		int last_set     = -1;
		
		for ( int i = 0;  i <= 8;  ++i, mask >>= 1 )
		{
			if ( mask & 1 )
			{
				// This register's bit is set in the mask
				
				const bool first = last_set < 0;
				
				const bool consecutive = !first  &&  last_set == i - 1;
				
				if ( consecutive  &&  last_set == first_in_run )
				{
					*p++ = '-';
				}
				
				if ( !consecutive )
				{
					if ( !first )
					{
						*p++ = '/';
					}
					
					*p++ = c;
					*p++ = '0' + i;
					
					first_in_run = i;
				}
				
				last_set = i;
			}
			else if ( first_in_run >= 0  &&  last_set > first_in_run )
			{
				*p++ = c;
				*p++ = '0' + last_set;
				
				first_in_run = -1;
			}
			
			if ( mask == 0 )
			{
				break;
			}
		}
		
		*p = '\0';
		
		return p;
	}
	
	static char* get_register_set( char* buffer, uint16_t mask, bool reverse )
	{
		if ( reverse )
		{
			mask =  mask << 15
			     | (mask << 13 & 0x4000)
			     | (mask << 11 & 0x2000)
			     | (mask <<  9 & 0x1000)
			     | (mask <<  7 & 0x0800)
			     | (mask <<  5 & 0x0400)
			     | (mask <<  3 & 0x0200)
			     | (mask <<  1 & 0x0100)
			     | (mask >>  1 & 0x0080)
			     | (mask >>  3 & 0x0040)
			     | (mask >>  5 & 0x0020)
			     | (mask >>  7 & 0x0010)
			     | (mask >>  9 & 0x0008)
			     | (mask >> 11 & 0x0004)
			     | (mask >> 13 & 0x0002)
			     |  mask >> 15;
		}
		
		const uint16_t low  = mask & 0xff;
		const uint16_t high = mask >>   8;
		
		char *p = buffer;
		
		p = get_register_sequence( p, low,  'D' );
		
		if ( !low == !high )
		{
			*p++ = '/';
		}
		
		p = get_register_sequence( p, high, 'A' );
		
		return p;
	}
	
	static void decode_long_mul_div( uint16_t op )
	{
		const uint16_t extension = read_word();
		
		const bool division = op & 0x0040;
		
		const bool is_signed = extension & 0x0800;
		const bool is_64_bit = extension & 0x0400;
		
		const char* qualifier = division && !is_64_bit ? "L.L" : ".L ";
		
		const uint16_t mode_reg = op & 0x3f;
		
		const uint16_t d_base = extension >> 24 & 0x7;
		const uint16_t d_more = extension       & 0x7;
		
		const plus::string ea = read_ea( mode_reg, 4 );
		
		char other_reg_name[] = "Dm:";
		
		other_reg_name[ 1 ] = '0' + d_more;
		
		const char* extra = is_64_bit  ||  d_more != d_base ? other_reg_name
		                                                    : "";
		
		const char* basename = division ? "DIV" : "MUL";
		
		char sign = is_signed ? 'S' : 'U';
		
		const char* format = "%s%c%s  %s,%sD%c" "\n";
		
		printf( format, basename, sign, qualifier, ea.c_str(), extra, '0' + d_base );
	}
	
	static void decode_MOVEM( uint16_t op )
	{
		const bool restore = op & 0x0400;
		const bool longs   = op & 0x0040;
		
		const uint16_t mode_reg = op & 0x3f;
		
		const bool reversed = !restore  &&  (mode_reg >> 3) == 4;
		
		const short size_index = longs + 1;
		
		const uint16_t mask = read_word();
		
		const short buffer_size = 16 * 3;  // 3 bytes per register max
		
		char buffer[ buffer_size ];
		
		char* end = get_register_set( buffer, mask, reversed );
		
		const char size_code = size_codes[ size_index ];
		
		const char* format = "MOVEM.%c  %s,%s" "\n";
		
		const plus::string ea = read_ea( mode_reg, sizes[ size_index ] );
		
		const char* source =  restore ? ea.c_str() : buffer;
		const char* dest   = !restore ? ea.c_str() : buffer;
		
		printf( format, size_code, source, dest );
	}
	
	static const char* const control_registers_000[] =
	{
		"SFC",
		"DFC",
		"CACR",
		"TC",    // 68040
		"ITT0",  // 68040
		"ITT1",  // 68040
		"DTT0",  // 68040
		"DTT1"   // 68040
	};
	
	static const char* const control_registers_800[] =
	{
		"USP",
		"VBR",
		"CAAR",   // 68020, 68030
		"MSP",
		"ISP",
		"MMUSR",  // 68040
		"URP",    // 68040
		"SRP"     // 68040
	};
	
	static void decode_MOVEC( uint16_t op )
	{
		const bool to = op & 0x0001;
		
		const uint16_t extension = read_word();
		
		const char bank = extension & 0x8000 ? 'A' : 'D';
		
		const uint16_t reg = extension >> 12 & 0x7;
		
		const uint16_t control = extension & 0x0FFF;
		
		if ( control & ~0x0807 )
		{
			throw illegal_instruction();
		}
		
		const char* const* register_set = control & 0x0800 ? control_registers_800
		                                                   : control_registers_000;
		
		const char* control_register_name = register_set[ control & 0x7 ];
		
		if ( to )
		{
			printf( "MOVEC    %c%d,%s" "\n", bank, reg, control_register_name );
		}
		else
		{
			printf( "MOVEC    %s,%c%d" "\n", control_register_name, bank, reg );
		}
	}
	
	static void decode_4e_misc( uint16_t op )
	{
		switch ( op & 0x0038 )
		{
			case 0x00:
			case 0x08:
				printf( "TRAP     #%#x" "\n", op & 0xf );
				break;
			
			case 0x10:
				printf( "LINK     A%d,#%d" "\n", op & 0x7, read_word_signed() );
				break;
			
			case 0x18:
				printf( "UNLK     A%d" "\n", op & 0x7 );
				break;
			
			case 0x20:
				printf( "MOVE     A%d,USP" "\n", op & 0x7 );
				break;
			
			case 0x28:
				printf( "MOVE     USP,A%d" "\n", op & 0x7 );
				break;
			
			case 0x30:
				const char* name;
				
				int arg;
				
				name = ops_4e7x[ op & 0x7 ];
				
				switch ( op & 0x7 )
				{
					case 2:  // STOP
					case 4:  // RTD
						arg = read_word_signed();
						break;
					
					default:
						arg = 0;  // not used, but needed to silence warning
				}
				
				printf( name, arg );
				
				switch ( op & 0x7 )
				{
					case 5:  // RTS
						if ( check_branch_target( global_bytes_read ) )
						{
							// Don't insert a newline if the next instruction is
							// a branch target
							break;
						}
						
						// fall through
					
					case 3:  // RTE
					case 4:  // RTD
					case 7:  // RTR
						global_successor_of_last_exit = global_bytes_read;
					
						printf( "\n" );
						break;
				}
				
				break;
			
			case 0x38:
				if ( (op & 0x000E) == 0x000A )
				{
					decode_MOVEC( op );
					break;
				}
				
			default:
				decode_default( op );
				break;
		}
	}
	
#pragma mark -
#pragma mark ** High-order **
	
	static void decode_data( uint16_t op )
	{
		printf( "DC.W     %#.4x", op );
		
		if ( globally_attach_target_comments )
		{
			printf( "  // %d bytes of data", op );
		}
		
		printf( "\n" );
		
		int n_words = (op + 1) / 2;
		
		while ( --n_words >= 0 )
		{
			if ( globally_prefix_address )
			{
				printf( "%.6x:  ", global_bytes_read );
			}
			
			if ( n_words-- )
			{
				printf( "DC.L     %#.8x\n", read_long() );
			}
			else
			{
				printf( "DC.W     %#.4x\n", read_word() );
			}
		}
		
		printf( "\n" );
	}
	
	static bool decoded_data( uint16_t op )
	{
		switch ( global_last_op )
		{
			case 0x4e75:  // RTS
			case 0xa9f4:  // _ExitToShell
				if ( op < 256 )
				{
					break;
				}
				else
				{
					// fall through
				}
			
			default:
				return false;
		}
		
		if ( op == 0  &&  peek_word() == 0 )
		{
			(void) read_word();
			
			printf( "DC.L     0x00000000\n\n" );
		}
		else if ( op != 0 )
		{
			decode_data( op );
		}
		else
		{
			return false;
		}
		
		return true;
	}
	
	static void decode_MOVEP( uint16_t op )
	{
		const bool store_to_mem = op & 0x80;
		const bool long_mode    = op & 0x40;
		
		const uint16_t data_reg = op >> 9 & 0x7;
		const uint16_t addr_reg = op >> 0 & 0x7;
		
		const uint16_t displacement = read_word();
		
		/*
			Some compilers are pedantic enough to issue an ERROR under
			`-Wnarrowing` [sic] since `'0' + data_reg` is technically an int,
			but never bother to notice that the result can't possibly overflow
			either int8_t or uint8_t.
		*/
		
		const char data_reg_char = char( '0' + data_reg );
		
		const char register_operand[ STRLEN( "Dx" ) ] = { 'D', data_reg_char };
		
		plus::var_string memory_operand = "(";
		
		memory_operand += gear::inscribe_decimal( displacement );
		
		memory_operand += ",A";
		
		memory_operand += '0' + addr_reg;
		
		memory_operand += ')';
		
		plus::var_string out = "MOVEP.";
		
		out += size_codes[ long_mode + 1 ];
		
		out += "  ";
		
		if ( store_to_mem )
		{
			out.append( register_operand, sizeof register_operand );
			
			out += ',';
			
			out += memory_operand;
		}
		else
		{
			out += memory_operand;
			
			out += ',';
			
			out.append( register_operand, sizeof register_operand );
		}
		
		printf( "%s\n", out.c_str() );
	}
	
	static void decode_MOVES( uint16_t op )
	{
		const uint16_t extension = read_word();
		
		const uint16_t mode_reg = op & 0x3f;
		
		const short size_index = op >> 6 & 0x3;
		
		if ( size_index == 3 )
		{
			throw illegal_instruction();
		}
		
		const char size_code = size_codes[ size_index ];
		
		const char bank = extension & 0x8000 ? 'A' : 'D';
		
		const uint16_t reg = extension >> 12 & 0x7;
		
		const plus::string ea = read_ea( mode_reg, sizes[ size_index ] );
		
		const bool to = extension & 0x0800;
		
		if ( to )
		{
			printf( "MOVES.%c  %c%d,%s\n", size_code, bank, reg, ea.c_str() );
		}
		else
		{
			printf( "MOVES.%c  %s,%c%d\n", size_code, ea.c_str(), bank, reg );
		}
	}
	
	static void decode_0_line( uint16_t op )
	{
		if ( const bool data = decoded_data( op ) )
		{
			return;
		}
		
		if ( op & 0x0100 )
		{
			if ( (op & 0x0038) == 0x0008 )
			{
				decode_MOVEP( op );
				
				return;
			}
			
			decode_Bit_op( op, true );  // BTST/BCHG/BCLR/BSET dynamic
			
			return;
		}
		
		switch ( op >> 8 & 0xf )
		{
			case 0x0:  // ORI
			case 0x2:  // ANDI
			case 0x4:  // SUBI
			case 0x6:  // ADDI
			case 0xa:  // EORI
			case 0xc:  // CMPI
				decode_Immediate( op );  // also CMP2/CHK2/CAS/CAS2
				break;
			
			case 0x8:  // BTST/BCHG/BCLR/BSET static
				decode_Bit_op( op, false );
				break;
			
			case 0xe:  // MOVES
				decode_MOVES( op );
				break;
			
			default:
				decode_default( op );
				break;
		};
	}
	
	static void decode_MOVE_Byte( uint16_t op )
	{
		decode_MOVE( op, 0 );
	}
	
	static void decode_MOVE_Long( uint16_t op )
	{
		decode_MOVE( op, 2 );
	}
	
	static void decode_MOVE_Word( uint16_t op )
	{
		decode_MOVE( op, 1 );
	}
	
	static void decode_4_line( uint16_t op )
	{
		const uint16_t source = op & 0x3f;
		
		if ( (op & 0xfff8) == 0x49c0 )
		{
			printf( "EXTB.L   D%d" "\n", op & 0x7 );
			
			return;
		}
		
		if ( (op & 0x4180) == 0x4180 )
		{
			const bool lea = op & 0x0040;
			
			const uint16_t immediate_size = lea ? 0 : 2;
			
			const char* format = lea ? "LEA      %s,A%d"
			                         : "CHK.W    %s,D%d";
			
			const plus::string ea = read_ea( source, immediate_size );
			
			printf( format, ea.c_str(), op >> 9 & 0x7 );
			
			if ( globally_attach_target_comments  &&  lea  &&  source == 0x3a )
			{
				print_comment( global_last_pc_relative_target );
			}
			
			printf( "\n" );
			
			return;
		}
		
		if ( op & 0x0100 )
		{
			throw illegal_instruction();
		}
		
		switch ( op >> 8 & 0xf )
		{
			case 0x0:  // NEGX
			case 0x2:  // CLR
			case 0x4:  // NEG
			case 0x6:  // NOT
			case 0xa:  // TST
				if ( (op & 0x00c0) == 0x00c0 )
				{
					decode_4_line_special( op );
				}
				else
				{
					decode_unary( op );
				}
				
				break;
			
			case 0x8:
				if ( (op & 0x00c0) == 0x0000 )
				{
					const plus::string ea = read_ea( source, 1 );
					
					printf( "NBCD.B   %s" "\n", ea.c_str() );
					
					break;
				}
				else if ( (op & 0x38) == 0x00 )
				{
					// SWAP, EXT.[WL]
					const char* name = swap_ext[ op >> 6 & 0x3 ];
					
					printf( "%s    D%d" "\n", name, op & 0x7 );
					
					break;
				}
				else if ( (op & 0x00c0) == 0x0040 )
				{
					if ( const bool is_bkpt = (op & 0xFFF8) == 0x4848 )
					{
						// BKPT
						const uint16_t vector = op & 0x7;
						
						printf( "BKPT     #%d" "\n", vector );
						
						break;
					}
					
					// PEA
					const plus::string ea = read_ea( source, 0 );
					
					printf( "PEA      %s" "\n", ea.c_str() );
					
					break;
				}
				// else fall through
			case 0xc:
				
				if ( (op & 0xff80) == 0x4c00 )
				{
					decode_long_mul_div( op );
				}
				else if ( (op & 0xfb80) == 0x4880 )
				{
					decode_MOVEM( op );
				}
				else
				{
					throw illegal_instruction();
				}
				break;
			
			case 0xe:
				if ( op & 0x0080 )
				{
					decode_Jump( op );
				}
				else if ( op & 0x0040 )
				{
					decode_4e_misc( op );
				}
				else
				{
					throw illegal_instruction();
				}
				
				break;
			
			default:
				decode_default( op );
				break;
		};
	}
	
	static inline uint16_t get_quick_data( uint16_t x )
	{
		return (x - 1 & 0x7) + 1;
	}
	
	static void decode_Quick( uint16_t op )
	{
		const short size_index = op >> 6 & 0x3;
		
		if ( size_index == 3 )
		{
			const char* ccode = condition_codes[ op >> 8 & 0xf ];
			
			if ( (op & 0x38) == 0x08 )
			{
				// DBcc
				
				const short displacement = read_word();
				
				printf( "DB%s     D%d,*%+d", ccode, op & 0x7, displacement + 2 );
				
				if ( globally_attach_target_comments )
				{
					print_comment( global_pc + displacement );
				}
				
				printf( "\n" );
			}
			else
			{
				const plus::string ea = read_ea( op & 0x3f, 1 );
				
				printf( "S%s.B    %s" "\n", ccode, ea.c_str() );
			}
			
			return;
		}
		
		const uint16_t quick_data = get_quick_data( op >> 9 );
		
		const bool subtract = op & 0x0100;
		
		const char* name = subtract ? "SUB" : "ADD";
		
		const char* format = "%sQ.%c   #%d,%s" "\n";
		
		const plus::string ea = read_ea( op & 0x3f, sizes[ size_index ] );
		
		printf( format, name, size_codes[ size_index ], quick_data, ea.c_str() );
	}
	
	static void decode_Branch( uint16_t op )
	{
		const uint32_t bytes_read = global_bytes_read;
		
		const uint16_t index = op >> 8 & 0xf;
		
		const char* ccode = index == 0 ? "RA"
		                  : index == 1 ? "SR"
		                  :              condition_codes[ index ];
		
		const unsigned char inline_arg = op & 0xff;
		
		const char* qualifier = inline_arg == 0xff ? ".L"
		                      : inline_arg == 0x00 ? "  "
		                      :                      ".S";
		
		const int arg = inline_arg == 0xff ? read_long()
		              : inline_arg == 0x00 ? read_word_signed()
		              :                      sign_extend_char( inline_arg );
		
		if ( arg & 1 )
		{
			throw illegal_operand();
		}
		
		/*
		const int sign_mask = inline_arg == 0xff ? 0x80000000
		                    : inline_arg == 0x00 ? 0x8000
		                    :                      0x80;
		
		const bool negative = arg & sign_mask;
		
		const char sign = negative ? '-' : '+';
		*/
		
		const uint32_t target = bytes_read + arg;
		
		global_last_branch_target = target;
		
		printf( "B%s%s    *%+d", ccode, qualifier, arg + 2 );
		
		if ( globally_attach_target_comments )
		{
			print_comment( target );
		}
		
		printf( "\n" );
		
		if ( index != 1 )
		{
			add_branch_target( target );
		}
		
		if ( index == 0 )
		{
			add_exit_point( global_bytes_read );
		}
		
		add_entry_point( target );
	}
	
	static void decode_MOVEQ( uint16_t op )
	{
		if ( op & 0x0100 )
		{
			throw illegal_instruction();
		}
		
		const signed char inline_arg = op & 0xff;
		
		const int arg = inline_arg;
		
		const int reg = op >> 9 & 0x7;
		
		printf( "MOVEQ    #%d,D%d" "\n", arg, reg );
	}
	
	static const char* sbcd_ops[] =
	{
		"SBCD.B   D%d,D%d"       "\n",
		"SBCD.B   -(A%d),-(A%d)" "\n"
	};
	
	static void decode_8_line( uint16_t op )
	{
		const uint16_t size_index = op >> 6 & 0x3;
		
		const uint16_t reg = op >> 9 & 0x7;
		
		if ( size_index == 3 )
		{
			const bool signed_math = op & 0x0100;
			
			const char sign_code = signed_math ? 'S' : 'U';
			
			const plus::string ea = read_ea( op & 0x3f, 2 );
			
			printf( "DIV%c.W   %s,D%d" "\n", sign_code, ea.c_str(), reg );
			
			return;
		}
		
		if ( op & 0x0100 )
		{
			uint16_t op_mode = op >> 3 & 0x1f;
			
			const char* format = NULL;
			
			switch ( op_mode )
			{
				case  0:  // SBCD.B
				case  1:  // SBCD.B
					format = sbcd_ops[ op_mode ];
					break;
				
				default:
					break;
			}
			
			if ( format )
			{
				printf( format, op & 0x7, reg );
				
				return;
			}
		}
		
		const char size_code = size_codes[ size_index ];
		
		const plus::string ea = read_ea( op & 0x3f, sizes[ size_index ] );
		
		if ( op & 0x0100 )
		{
			printf( "OR.%c     D%d,%s" "\n", size_code, reg, ea.c_str() );
		}
		else
		{
			printf( "OR.%c     %s,D%d" "\n", size_code, ea.c_str(), reg );
		}
	}
	
	static void decode_B_line( uint16_t op )
	{
		const uint16_t size_bits = op >> 6 & 0x3;
		
		const uint16_t reg = op >> 9 & 0x7;
		
		const uint16_t size_index = size_bits < 3 ? size_bits
		                          : op & 0x0100   ? 2
		                          :                 1;
		
		const char size_code = size_codes[ size_index ];
		
		if ( size_bits != 3  &&  (op & 0x0138) == 0x0108 )
		{
			printf( "CMPM.%c   (A%d)+,(A%d)+" "\n", size_code, op & 0x7, reg );
			
			return;
		}
		
		const plus::string ea = read_ea( op & 0x3f, sizes[ size_index ] );
		
		if ( size_bits == 3 )
		{
			printf( "CMPA.%c   %s,A%d" "\n", size_code, ea.c_str(), reg );
		}
		else if ( op & 0x0100 )
		{
			printf( "EOR.%c    D%d,%s" "\n", size_code, reg, ea.c_str() );
		}
		else
		{
			printf( "CMP.%c    %s,D%d" "\n", size_code, ea.c_str(), reg );
		}
	}
	
	static const char* exg_abcd_ops[] =
	{
		"ABCD.B   D%d,D%d" "\n",
		"ABCD.B   -(A%d),-(A%d)" "\n",
		"EXG      D%d,D%d" "\n",
		"EXG      A%d,A%d" "\n",
		"",
		"EXG      D%d,A%d" "\n",
	};
	
	static void decode_C_line( uint16_t op )
	{
		const uint16_t size_index = op >> 6 & 0x3;
		
		const uint16_t reg = op >> 9 & 0x7;
		
		if ( size_index == 3 )
		{
			const bool signed_math = op & 0x0100;
			
			const char sign_code = signed_math ? 'S' : 'U';
			
			const plus::string ea = read_ea( op & 0x3f, 2 );
			
			printf( "MUL%c.W   %s,D%d" "\n", sign_code, ea.c_str(), reg );
			
			return;
		}
		
		if ( op & 0x0100 )
		{
			const uint16_t op_mode = op >> 3 & 0x1f;
			
			const char* format = NULL;
			
			switch ( op_mode )
			{
				case  0:  // ABCD.B
				case  1:  // ABCD.B
				case  8:  // EXG
				case  9:  // EXG
				case 17:  // EXG
					format = exg_abcd_ops[ op_mode >> 2 | (op_mode & 1) ];
					break;
				
				default:
					break;
			}
			
			if ( format )
			{
				if ( op_mode >> 1 )
				{
					printf( format, reg, op & 0x7 );
				}
				else
				{
					printf( format, op & 0x7, reg );
				}
				
				return;
			}
		}
		
		const char size_code = size_codes[ size_index ];
		
		const plus::string ea = read_ea( op & 0x3f, sizes[ size_index ] );
		
		if ( op & 0x0100 )
		{
			printf( "AND.%c    D%d,%s" "\n", size_code, reg, ea.c_str() );
		}
		else
		{
			printf( "AND.%c    %s,D%d" "\n", size_code, ea.c_str(), reg );
		}
	}
	
	static void decode_ADD_SUB( uint16_t op )
	{
		const bool adding = op & 0x4000;
		
		const char* name = adding ? "ADD" : "SUB";
		
		const uint16_t reg = op >> 9 & 0x7;
		
		uint16_t size_index = op >> 6 & 0x3;
		
		const bool adda = size_index == 3;
		
		if ( adda )
		{
			size_index = (op >> 8 & 0x01) + 1;
		}
		
		const char size_code = size_codes[ size_index ];
		
		if ( const bool addx = (op & 0x0130) == 0x0100  &&  !adda )
		{
			const char* format = op & 0x08 ? "%sX.%c   -(A%d),-(A%d)" "\n"
			                               : "%sX.%c   D%d,D%d" "\n";
			
			printf( format, name, size_code, op & 0x7, reg );
			
			return;
		}
		
		const plus::string ea = read_ea( op & 0x3f, sizes[ size_index ] );
		
		if ( adda )
		{
			const char* format = "%sA.%c   %s,A%d" "\n";
			
			printf( format, name, size_code, ea.c_str(), reg );
			
			return;
		}
		
		if ( const bool reversed = op & 0x0100 )
		{
			printf( "%s.%c    D%d,%s" "\n", name, size_code, reg, ea.c_str() );
		}
		else
		{
			printf( "%s.%c    %s,D%d" "\n", name, size_code, ea.c_str(), reg );
		}
	}
	
	static
	void decode_bitfield( uint16_t op )
	{
		const uint16_t extension = read_word();
		
		const plus::string ea = read_ea( op & 0x3f, 0 );
		
		const short offset = extension >> 6 & 0x1f;
		const short width  = extension      & 0x1f;
		
		const short Do = extension & 0x0800;
		const short Dw = extension & 0x0020;
		const short is_bfonly = op & 0x0100;
		
		const char* const* ops = is_bfonly ? bfonly_ops : bit_ops;
		
		const char* partial_mnemonic = ops[ op >> 9 & 0x3 ];
		
		/*
			The partial mnemonic is either 3 or 4 characters.  For the minimum
			of 3, append the maximum 4 space characters.  For the longer 4,
			append only 3 space characters.
		*/
		
		const char* space = &"    "[ strlen( partial_mnemonic ) - 3 ];
		
		printf( "BF%s%s", partial_mnemonic, space );
		
		if ( (op & 0x0f00) == 0x0F00 )
		{
			// BFINS
			
			printf( "D%c,", '0' | extension >> 12 );
		}
		
		const char o = Do ? 'D' : '#';
		const char w = Dw ? 'D' : '#';
		
		printf( "%s {%c%d:%c%d}", ea.c_str(), o, offset, w, width );
		
		if ( op & 0x0100  &&  (op & 0x0f00) != 0x0F00 )
		{
			// BFEXTU, BFEXTS, BFFFO
			
			printf( ",D%c", '0' | extension >> 12 );
		}
		
		printf( "\n" );
	}
	
	static void decode_shift_rotate( uint16_t op )
	{
		const short size_index = op >> 6 & 0x3;
		
		if ( size_index == 3  &&  op & 0x800 )
		{
			if ( op & 0x0800 )
			{
				decode_bitfield( op );
				return;
			}
			
			throw illegal_instruction();
		}
		
		const bool left = op & 0x100;
		
		const bool uses_ea = size_index == 3;
		
		const short op_index = op >> (uses_ea ? 9 : 3 ) & 0x3;
		
		const char* op_name = bit_slide_ops[ op_index ];
		
		const char direction = left ? 'L' : 'R';
		
		const char* space = "      ";
		
		if ( op_index == 2 )
		{
			++space;  // ROX
		}
		
		printf( "%s%c", op_name, direction );
		
		if ( !uses_ea )
		{
			printf( ".%c", size_codes[ size_index ] );
			
			space += 2;
		}
		
		printf( "%s", space );
		
		if ( uses_ea )
		{
			const plus::string ea = read_ea( op & 0x3f, 0 );
			
			printf( "%s", ea.c_str() );
		}
		else
		{
			const bool count_in_Dn = op & 0x20;
			
			const char* format = count_in_Dn ? "D%d,D%d" : "#%d,D%d";
			
			const uint16_t source = op >> 9 & 0x7;
			
			const uint16_t count = count_in_Dn ? source : get_quick_data( source );
			
			printf( format, count, op & 0x7 );
		}
		
		printf( "\n" );
	}
	
	static void decode_A_line( uint16_t op )
	{
		const char* name = get_aTrap_name( op );
		
		if ( name )
		{
			printf( "%s" "\n", name );
		}
		else if ( op >= 0xA800  &&  (name = get_aTrap_name( op & ~0x0400 )) )
		{
			printf( "%s,AutoPop" "\n", name );
		}
		else if ( op < 0xA800  &&  (name = get_aTrap_name( op & ~0x0600 )) )
		{
			const char* sys   = op & 0x0400 ? ",Sys"   : "";
			const char* immed = op & 0X0200 ? ",Immed" : "";
			
			printf( "%s%s%s" "\n", name, sys, immed );
		}
		else
		{
			decode_default( op );
		}
		
		if ( op == 0xa9f4 )
		{
			global_successor_of_last_exit = global_bytes_read;
		}
	}
	
	static void decode_F_line( uint16_t op )
	{
		if ( op == 0xf210 )
		{
			const uint16_t extension = read_word();
			
			if ( (extension & 0xfc7f) == 0x4800 )
			{
				const short fp = extension >> 7 & 0x7;
				
				printf( "FMOVE.X  (A0),FP%d\n", fp );
				
				return;
			}
		}
		
		decode_default( op );
	}
	
	const decoder mask_of_4_bits[] =
	{
		decode_0_line,
		decode_MOVE_Byte,
		decode_MOVE_Long,
		decode_MOVE_Word,
		decode_4_line,
		decode_Quick,
		decode_Branch,
		decode_MOVEQ,
		decode_8_line,
		decode_ADD_SUB,  // SUB
		decode_A_line,
		decode_B_line,
		decode_C_line,
		decode_ADD_SUB,  // ADD
		decode_shift_rotate,
		decode_F_line,
	};
	
	
	static inline uint32_t n_ones( int n )
	{
		return (1 << n) - 1;
	}
	
	static const uint32_t name_validity[] =
	{
		0,  // no control chars
		
		            1 << (' ' & 31) |  // 0x20
		            1 << ('%' & 31) |  // 0x25
		            1 << ('.' & 31) |  // 0x2e
		n_ones( 10 )  << ('0' & 31),   // 0x30 - 0x39
		
		n_ones( 26 )  << ('A' & 31) |  // 0x41 - 0x5A
		            1 << ('_' & 31),   // 0x5f
		
		n_ones( 26 )  << ('a' & 31)    // 0x61 - 0x7A
	};
	
	static inline bool valid_name_char( unsigned char c )
	{
		return name_validity[ c >> 5 & 0x3 ] & 1 << (c & 0x1f);
	}
	
	static const char* get_name( uint16_t word )
	{
		const uint16_t byte_0 = word >> 8;
		const uint16_t byte_1 = word & 0xff;
		
		if ( byte_0 < 0x80 )
		{
			return NULL;
		}
		
		static char name[ 256 ];
		
		char* p = name;
		
		uint32_t length = 0;
		
		const bool try_fixed = false;
		
		if ( const bool is_fixed_length = byte_0 >= (0x80 | 0x20) )
		{
			if ( !valid_name_char( byte_0 )  ||  !valid_name_char( byte_1 ) )
			{
				return NULL;
			}
			
			const bool is_method = byte_1 & 0x80;
			
			*p++ = byte_0 & 0x7f;
			*p++ = byte_1 & 0x7f;
			
			length = 8 + 8 * is_method - 2;
		}
		else if ( const uint32_t length_byte = byte_0 & 0x1f )
		{
			if ( !valid_name_char( byte_1 ) )
			{
				return NULL;
			}
			
			*p++ = byte_1;
			
			length = length_byte - 1;
		}
		else
		{
			length = byte_1;
		}
		
		(void) read_word();
		
		for ( p[ length ] = '\0';  length > 1;  length -= 2 )
		{
			const uint16_t pair = read_word();
			
			*p++ = pair >> 8;
			*p++ = pair & 0xff;
		}
		
		if ( length )
		{
			*p++ = read_word() >> 8;
		}
		
		return name;
	}
	
	static void decode_one()
	{
		if ( global_bytes_read == global_successor_of_last_exit )
		{
			// Check for Macsbug symbol names
			
			const uint16_t word_0 = peek_word();
			
			if ( const char* name = get_name( word_0 ) )
			{
				printf( "// ^^^ %s\n\n", name );
				
				if ( peek_word() <= 256 )
				{
					decode_data( read_word() );
				}
				
				return;
			}
		}
		
		if ( globally_prefix_address )
		{
			printf( "%.6x:  ", global_bytes_read );
		}
		
		const uint16_t word = read_word();
		
		global_pc = global_bytes_read;
		
		if ( const decoder decode = mask_of_4_bits[ word >> 12 ] )
		{
			try
			{
				decode( word );
				
				global_last_op = word;
				
				return;
			}
			catch ( const illegal_instruction& )
			{
				printf( "Illegal instruction" "\n" );
			}
			catch ( const illegal_operand& )
			{
				printf( "Illegal operand" "\n" );
			}
		}
		
		decode_default( word );
	}
	
	static void skip_resource_header()
	{
		const uint16_t  flags   = read_word();
		const uint32_t  type    = read_long();
		const uint16_t  id      = read_word();
		const uint16_t  version = read_word();
		
		printf( "// Flags:          %d" "\n", flags   );
		
		printf( "// Resource type:  '%c%c%c%c'" "\n", type >> 24,
		                                              type >> 16,
		                                              type >>  8,
		                                              type );
		
		printf( "// Resource id:    %d" "\n", id      );
		printf( "// Version:        %d" "\n", version );
	}
	
	int Main( int argc, char** argv )
	{
		char *const *args = get_options( argv );
		
		globally_prefix_address         = ! quiet;
		globally_attach_target_comments = ! quiet;
		
		const int argn = argc - (args - argv);
		
		if ( argn > 0 )
		{
			p7::dup2( p7::open( args[ 0 ], p7::o_rdonly ), p7::stdin_fileno );
		}
		
		global_entry_points.push_back( 0 );  // default entry point
		
		try
		{
			decode_one();
			
			if ( global_last_branch_target == 12 )
			{
				skip_resource_header();
			}
			
			while ( true )
			{
				decode_one();
			}
		}
		catch ( const end_of_file& )
		{
			printf( "\n" );
		}
		
		return 0;
	}
	
}
