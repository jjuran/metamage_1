/*	=======
 *	echo.cc
 *	=======
 */

// Standard C++
#include <vector>

// Standard C
#include <ctype.h>
#include <stdio.h>
#include <string.h>

// Iota
#include "iota/decimal.hh"
#include "iota/hexidecimal.hh"
#include "iota/strings.hh"

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
	
	Line 0:  missing MOVEP and MOVES
	Line 1:  complete  (MOVE.B)
	Line 2:  complete  (MOVE.L)
	Line 3:  complete  (MOVE.W)
	Line 4:  missing MOVEC
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


namespace tool
{
	
	namespace p7 = poseven;
	
	
	#define COMMENT  "    ; "
	
	
	struct str_len
	{
		const char*  string;
		size_t       length;
	};
	
	struct string_length
	{
		const char*  string;
		size_t       length;
		
		string_length( const char* s, size_t len ) : string( s ), length( len )
		{
		}
	};
	
	static inline int sign_extend_char( signed char x )
	{
		return x;
	}
	
	static inline unsigned short byte_swap( unsigned short x )
	{
	#ifdef __LITTLE_ENDIAN__
		
		x = x << 8 | x >> 8;
		
	#endif
		
		return x;
	}
	
	static void append_hex( plus::var_string& s, unsigned long x, int min_bytes )
	{
		const unsigned short min_digits = min_bytes * 2;
		
		const unsigned short magnitude = iota::hexidecimal_magnitude( x );
		
		const unsigned short even_magnitude = magnitude + (magnitude & 0x1 );
		
		const unsigned short n_bytes = std::max( even_magnitude, min_digits );
		
		s.resize( s.size() + n_bytes );
		
		char* buf = &*s.end() - n_bytes;
		
		iota::inscribe_n_hex_digits( buf, x, n_bytes );
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
		
		s += iota::inscribe_decimal( x );
	}
	
	static inline const char* get_aTrap_name( unsigned short trap_word )
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
	
	
	static bool globally_prefix_address         = true;
	static bool globally_attach_target_comments = true;
	
	static size_t global_bytes_read = 0;
	
	static size_t global_pc = 0;
	
	static size_t global_last_branch_target = 0;
	static size_t global_last_pc_relative_target = 0;
	static size_t global_successor_of_last_exit = 0;
	
	static std::vector< size_t > global_branch_targets;
	
	static std::vector< size_t > global_entry_points;
	static std::vector< size_t > global_exit_points;
	
	
	static void add_branch_target( size_t address )
	{
		typedef std::vector< size_t >::iterator iterator;
		
		const iterator it = std::lower_bound( global_branch_targets.begin(),
		                                      global_branch_targets.end(),
		                                      address );
		
		const bool found = it != global_branch_targets.end()  &&  *it == address;
		
		if ( !found )
		{
			global_branch_targets.insert( it, address );
		}
	}
	
	static bool check_branch_target( size_t address )
	{
		typedef std::vector< size_t >::iterator iterator;
		
		const iterator it = std::lower_bound( global_branch_targets.begin(),
		                                      global_branch_targets.end(),
		                                      address );
		
		const bool found = it != global_branch_targets.end()  &&  *it == address;
		
		global_branch_targets.erase( global_branch_targets.begin(), it );
		
		return found;
	}
	
	static void add_exit_point( size_t address )
	{
		global_exit_points.push_back( address );
	}
	
	static void add_entry_point( size_t address )
	{
		// Add an entry point unless
		// * it already exists
		// * it lies between an entry point and an exit point (or eof)
		
		typedef std::vector< size_t >::iterator iterator;
		
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
		
		const size_t previous_exit = *it;
		
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
	
	static unsigned short read_word( bool peeking = false )
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
		
		const unsigned short result = byte_swap( *(unsigned short *) p );
		
		if ( !peeking )
		{
			p += sizeof (unsigned short);
			
			global_bytes_read += sizeof (unsigned short);
		}
		
		return result;
	}
	
	static inline unsigned short peek_word()
	{
		return read_word( true );
	}
	
	static inline short read_word_signed()
	{
		return read_word();
	}
	
	static unsigned int read_long()
	{
		const unsigned int high = read_word();
		const unsigned int low  = read_word();
		
		return high << 16 | low;
	}
	
	
	static void set_register_name( char* name, short mode, short n )
	{
		name[0] = mode == 0 ? 'D' : 'A';
		name[1] = '0' + n;
	}
	
	static int read_extended_displacement( unsigned short size_code )
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
	
	static unsigned global_last_immediate_data_from_ea = 0;
	
	static plus::string read_ea( short mode_reg, short immediate_size )
	{
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
		
		unsigned short extension = read_word();
		
		if ( mode == 5  ||  mode == 7 && reg == 2 )
		{
			// Address Register Indirect with Displacement
			// Program Counter Indirect with Displacement
			
			const short displacement = extension;
			
			if ( const bool pc_relative = mode == 7  &&  immediate_size == 0 )
			{
				result += '*';
				
				append_signed_decimal( result, displacement );
				
				global_last_pc_relative_target = global_pc + displacement;
			}
			else
			{
				result += "(";
				
				if ( displacement )
				{
					result += iota::inscribe_decimal( displacement );
					
					result += ",";
				}
				
				result += reg_name;
				result += ")";
			}
		}
		else if ( mode == 6  ||  mode == 7 && reg == 3 )
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
				result += iota::inscribe_decimal( base_displacement );
				
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
					result += "0x";
					append_hex( result, extension, 2 );
					
					if ( reg == 1 )
					{
						append_hex( result, read_word(), 2 );
					}
					
					break;
				
				case 4:
					result += "#0x";
					
					unsigned immediate_data;
					
					immediate_data = immediate_size == 1 ? extension & 0xff
					               : immediate_size == 2 ? extension
					               :                       extension << 16 | read_word();
					
					append_hex( result, immediate_data, immediate_size );
					
					global_last_immediate_data_from_ea = immediate_data;
					
					break;
				
				default:
					throw illegal_operand();
					break;
			}
		}
		
		return result;
	}
	
	
	typedef void (*decoder)( unsigned short op );
	
	static void decode_default( unsigned short op )
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
#pragma mark ¥ Line 0 ¥
	
	static void decode_Immediate( unsigned short op )
	{
		const short size_index = op >> 6 & 0x3;
		
		if ( size_index == 3 )
		{
			throw illegal_instruction();
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
		
		unsigned immediate_data = read_word();
		
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
			
			const plus::string ea = read_ea( mode_reg, immediate_size );
			
			printf( format, name, qualifier, space, immediate_data, ea.c_str() );
		}
	}
	
	static void decode_Bit_op( unsigned short op )
	{
		char format[] = "Bfoo     #%#x,%s" "\n";
		
		const char* name = bit_ops[ op >> 6 & 0x3 ];
		
		const size_t name_len = STRLEN( "foo" );
		
		memcpy( format + STRLEN( "B" ), name, name_len );
		
		const short mode_reg = op & 0x3f;
		
		const unsigned immediate_data = read_word();
		
		const short immediate_size = 1;
		
		const plus::string ea = read_ea( mode_reg, immediate_size );
		
		printf( format, immediate_data, ea.c_str() );
	}
	
#pragma mark -
#pragma mark ¥ Line 1-3 ¥
	
	static void decode_MOVE( unsigned short op, short size_index )
	{
		const short immediate_size = sizes[ size_index ];
		
		const short source_mode_reg = op & 0x3f;
		
		const short dest_mode_reg = (op >> 3 & 0x38) | (op >> 9 & 0x07);
		
		plus::var_string comment;
		
		if ( immediate_size == 2  &&  dest_mode_reg == 0  &&  source_mode_reg == 0x3c )
		{
			const unsigned short data = peek_word();
			
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
			const unsigned data = global_last_immediate_data_from_ea;
			
			if (     isprint( data >> 24        )
			     &&  isprint( data >> 16 & 0xff )
			     &&         ( data >>  8 & 0xff ) >= ' '
			     &&         ( data       & 0xff ) >= ' ' )
			{
				comment = COMMENT;
				
				const char osType[] =
				{
					'\'',
					data >> 24,
					data >> 16 & 0xff,
					data >>  8 & 0xff,
					data       & 0xff,
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
#pragma mark ¥ Line 4 ¥
	
	static const str_len unary_ops[] =
	{
		{ STR_LEN( "NEGX" ) },
		{ STR_LEN( "CLR"  ) },
		{ STR_LEN( "NEG"  ) },
		{ STR_LEN( "NOT"  ) },
		{ STR_LEN( ""     ) },
		{ STR_LEN( "TST"  ) },
		{ STR_LEN( ""     ) },
		{ STR_LEN( ""     ) }
	};
	
	static void decode_unary( unsigned short op )
	{
		const short size_index = op >> 6 & 0x3;
		
		if ( size_index == 3 )
		{
			throw illegal_instruction();
		}
		
		char format[] = "%s.%c    %s" "\n";
		
		const str_len name = unary_ops[ op >> 9 & 0x7 ];
		
		if ( op & 0x0100  ||  name.length == 0 )
		{
			throw illegal_instruction();
		}
		
		const short immediate_size = sizes[ size_index ];
		
		const plus::string ea = read_ea( op & 0x3f, immediate_size );
		
		printf( format, name.string, size_codes[ size_index ], ea.c_str() );
	}
	
	static const char* move_ccr_sr[] =
	{
		"MOVE     SR,%s"  "\n",
		"MOVE     CCR,%s" "\n",
		"MOVE     %s,CCR" "\n",
		"MOVE     %s,SR"  "\n"
	};
	
	static void decode_4_line_special( unsigned short op )
	{
		if ( op == 0x4afc )
		{
			printf( "%s" "\n", "ILLEGAL" );
			
			return;
		}
		
		const bool tas = op & 0x0f00 == 0x0a00;
		
		const unsigned short immediate_size = tas ? 1 : 2;
		
		const plus::string ea = read_ea( op & 0x3f, immediate_size );
		
		const char* format = tas ? "TAS.B    %s" "\n"
		                         : move_ccr_sr[ op >> 9 & 0x3 ];
		
		printf( format, ea.c_str() );
	}
	
	static bool jump_breaks_routine( unsigned short mode_reg )
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
	
	static void print_comment( unsigned long pc_relative_target )
	{
		printf( COMMENT "%#.6x", pc_relative_target );
	}
	
	static void decode_Jump( unsigned short op )
	{
		const unsigned short source = op & 0x3f;
		
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
		
		printf( newlines );
		
		if ( jump )
		{
			global_successor_of_last_exit = global_bytes_read;
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
	
	static char* get_register_sequence( char* buffer, unsigned short mask, char c )
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
	
	static char* get_register_set( char* buffer, unsigned short mask, bool reverse )
	{
		if ( reverse )
		{
			mask = mask << 15
			     | mask << 13 & 0x4000
			     | mask << 11 & 0x2000
			     | mask <<  9 & 0x1000
			     | mask <<  7 & 0x0800
			     | mask <<  5 & 0x0400
			     | mask <<  3 & 0x0200
			     | mask <<  1 & 0x0100
			     | mask >>  1 & 0x0080
			     | mask >>  3 & 0x0040
			     | mask >>  5 & 0x0020
			     | mask >>  7 & 0x0010
			     | mask >>  9 & 0x0008
			     | mask >> 11 & 0x0004
			     | mask >> 13 & 0x0002
			     | mask >> 15;
		}
		
		const unsigned short low  = mask & 0xff;
		const unsigned short high = mask >>   8;
		
		char *p = buffer;
		
		p = get_register_sequence( p, low,  'D' );
		
		if ( !low == !high )
		{
			*p++ = '/';
		}
		
		p = get_register_sequence( p, high, 'A' );
		
		return p;
	}
	
	static void decode_long_mul_div( unsigned short op )
	{
		const unsigned short extension = read_word();
		
		const bool division = op & 0x0040;
		
		const bool is_signed = extension & 0x0800;
		const bool is_64_bit = extension & 0x0400;
		
		const char* qualifier = division && !is_64_bit ? "L.L" : ".L ";
		
		const unsigned short mode_reg = op & 0x3f;
		
		const unsigned short d_base = extension >> 24 & 0x7;
		const unsigned short d_more = extension       & 0x7;
		
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
	
	static void decode_MOVEM( unsigned short op )
	{
		const bool restore = op & 0x0400;
		const bool longs   = op & 0x0040;
		
		const unsigned short mode_reg = op & 0x3f;
		
		const bool reversed = !restore  &&  (mode_reg >> 3) == 4;
		
		const short size_index = longs + 1;
		
		const unsigned short mask = read_word();
		
		const size_t buffer_size = 16 * 3;  // 3 bytes per register max
		
		char buffer[ buffer_size ];
		
		char* end = get_register_set( buffer, mask, reversed );
		
		const char size_code = size_codes[ size_index ];
		
		const char* format = "MOVEM.%c  %s,%s" "\n";
		
		const plus::string ea = read_ea( mode_reg, sizes[ size_index ] );
		
		const char* source =  restore ? ea.c_str() : buffer;
		const char* dest   = !restore ? ea.c_str() : buffer;
		
		printf( format, size_code, source, dest );
	}
	
	static void decode_4e_misc( unsigned short op )
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
			default:
				decode_default( op );
				break;
		}
	}
	
#pragma mark -
#pragma mark ¥ High-order ¥
	
	static void decode_0_line( unsigned short op )
	{
		switch ( op >> 8 & 0xf )
		{
			case 0x0:  // ORI
			case 0x2:  // ANDI
			case 0x4:  // SUBI
			case 0x6:  // ADDI
			case 0xa:  // EORI
			case 0xc:  // CMPI
				decode_Immediate( op );
				break;
			
			case 0x8:  // BTST/BCHG/BCLR/BSET
				decode_Bit_op( op );
				break;
			
			case 0x1:  // MOVEP
				
				//break;
			
			case 0xe:  // MOVES
				
				//break;
			
			default:
				decode_default( op );
				break;
		};
	}
	
	static void decode_MOVE_Byte( unsigned short op )
	{
		decode_MOVE( op, 0 );
	}
	
	static void decode_MOVE_Long( unsigned short op )
	{
		decode_MOVE( op, 2 );
	}
	
	static void decode_MOVE_Word( unsigned short op )
	{
		decode_MOVE( op, 1 );
	}
	
	static void decode_4_line( unsigned short op )
	{
		const unsigned short source = op & 0x3f;
		
		if ( (op & 0xfff8) == 0x49c0 )
		{
			printf( "EXTB.L   D%d" "\n", op & 0x7 );
			
			return;
		}
		
		if ( (op & 0x4180) == 0x4180 )
		{
			const bool lea = op & 0x0040;
			
			const unsigned short immediate_size = lea ? 0 : 2;
			
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
	
	static inline unsigned short get_quick_data( unsigned short x )
	{
		return (x - 1 & 0x7) + 1;
	}
	
	static void decode_Quick( unsigned short op )
	{
		const short size_index = op >> 6 & 0x3;
		
		if ( size_index == 3 )
		{
			const char* ccode = condition_codes[ op >> 8 & 0xf ];
			
			if ( (op & 0x38) == 0x08 )
			{
				// DBcc
				
				const short displacement = read_word();
				
				printf( "DB%s     D%d,*%+d", ccode, op & 0x7, displacement );
				
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
		
		const unsigned short quick_data = get_quick_data( op >> 9 );
		
		const bool subtract = op & 0x0100;
		
		const char* name = subtract ? "SUB" : "ADD";
		
		const char* format = "%sQ.%c   #%d,%s" "\n";
		
		const plus::string ea = read_ea( op & 0x3f, sizes[ size_index ] );
		
		printf( format, name, size_codes[ size_index ], quick_data, ea.c_str() );
	}
	
	static void decode_Branch( unsigned short op )
	{
		const size_t bytes_read = global_bytes_read;
		
		const char index = op >> 8 & 0xf;
		
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
		
		const size_t target = bytes_read + arg;
		
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
	
	static void decode_MOVEQ( unsigned short op )
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
	
	static void decode_8_line( unsigned short op )
	{
		const unsigned short size_index = op >> 6 & 0x3;
		
		const unsigned short reg = op >> 9 & 0x7;
		
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
			unsigned short op_mode = op >> 3 & 0x1f;
			
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
	
	static void decode_B_line( unsigned short op )
	{
		unsigned short size_index = op >> 6 & 0x3;
		
		const unsigned short reg = op >> 9 & 0x7;
		
		if ( size_index == 3 )
		{
			size_index = op & 0x0100 ? 2 : 1;
		}
		
		const char size_code = size_codes[ size_index ];
		
		if ( size_index != 3  &&  (op & 0x0138) == 0x0108 )
		{
			printf( "CMPM.%c   (A%d)+,(A%d)+" "\n", size_code, op & 0x7, reg );
			
			return;
		}
		
		const plus::string ea = read_ea( op & 0x3f, sizes[ size_index ] );
		
		if ( size_index == 3 )
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
	
	static void decode_C_line( unsigned short op )
	{
		const unsigned short size_index = op >> 6 & 0x3;
		
		const unsigned short reg = op >> 9 & 0x7;
		
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
			const unsigned short op_mode = op >> 3 & 0x1f;
			
			const char* format = NULL;
			
			switch ( op_mode )
			{
				case  0:  // ABCD.B
				case  1:  // ABCD.B
				case  8:  // EXG
				case  9:  // EXG
				case 17:  // EXG
					format = exg_abcd_ops[ op_mode >> 2 | op_mode & 1 ];
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
	
	static void decode_ADD_SUB( unsigned short op )
	{
		const bool adding = op & 0x4000;
		
		const char* name = adding ? "ADD" : "SUB";
		
		const unsigned short reg = op >> 9 & 0x7;
		
		unsigned short size_index = op >> 6 & 0x3;
		
		const bool adda = size_index == 3;
		
		if ( adda )
		{
			size_index = (op >> 8 & 0x01) + 1;
		}
		
		const char size_code = size_codes[ size_index ];
		
		if ( const bool addx = (op & 0x0130) == 0x0100  &&  !adda )
		{
			const char* format = op & 0x08 ? "%sX.%c   %s-(A%d),-(A%d)" "\n"
			                               : "%sX.%c   %sD%d,D%d" "\n";
			
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
	
	static void decode_shift_rotate( unsigned short op )
	{
		const short size_index = op >> 6 & 0x3;
		
		if ( size_index == 3  &&  op & 0x800 )
		{
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
			
			const unsigned short source = op >> 9 & 0x7;
			
			const unsigned short count = count_in_Dn ? source : get_quick_data( source );
			
			printf( format, count, op & 0x7 );
		}
		
		printf( "\n" );
	}
	
	static void decode_A_line( unsigned short op )
	{
		const char* name = get_aTrap_name( op );
		
		if ( name )
		{
			printf( "%s" "\n", name );
		}
		else
		{
			decode_default( op );
		}
	}
	
	static void decode_F_line( unsigned short op )
	{
		if ( op == 0xf210 )
		{
			const unsigned short extension = read_word();
			
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
	
	
	static bool name_assumed( unsigned short word )
	{
		const unsigned short byte_0 = word >> 8;
		const unsigned short byte_1 = word & 0xff;
		
		if ( const bool long_name = byte_0 == 0x80 )
		{
			return true;
		}
		
		const bool name_allowed = (byte_0 & 0xe0) == 0x80;
		
		if ( !name_allowed )
		{
			return false;
		}
		
		//return byte_1 >= 0x20  &&  byte_1 < 0x7f;
		
		return byte_1 == '_'  ||  isalpha( byte_1 );
	}
	
	static void decode_one()
	{
		if ( global_bytes_read == global_successor_of_last_exit )
		{
			// Check for Macsbug symbol names
			
			const unsigned short word_0 = peek_word();
			
			if ( name_assumed( word_0 ) )
			{
				const unsigned short byte_0 = word_0 >> 8;
				const unsigned short byte_1 = word_0 & 0xff;
				
				const bool long_name = byte_0 == 0x80;
				
				const short length = long_name ? byte_1
				                               : byte_0 & 0x1f;
				
				char name[ 256 ];
				
				if ( !long_name )
				{
					name[0] = byte_1;
				}
				
				int i = long_name ? 0 : 1;
				
				(void) read_word();
				
				while ( i < length )
				{
					const unsigned short word = read_word();
					
					name[ i++ ] = word >> 8;
					
					if ( i == length )
					{
						break;
					}
					
					name[ i++ ] = word & 0xff;
				}
				
				name[ i ] = '\0';
				
				printf( "; ^^^ %s\n\n", name );
				
				(void) read_word();
				
				return;
			}
		}
		
		if ( globally_prefix_address )
		{
			printf( "%.6x:  ", global_bytes_read );
		}
		
		const unsigned short word = read_word();
		
		global_pc = global_bytes_read;
		
		if ( const decoder decode = mask_of_4_bits[ word >> 12 ] )
		{
			try
			{
				decode( word );
				
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
		const unsigned short  flags   = read_word();
		const unsigned long   type    = read_long();
		const unsigned short  id      = read_word();
		const unsigned short  version = read_word();
		
		printf( "; Flags:          %d" "\n", flags   );
		
		printf( "; Resource type:  '%c%c%c%c'" "\n", type >> 24,
		                                             type >> 16,
		                                             type >>  8,
		                                             type );
		
		printf( "; Resource id:    %d" "\n", id      );
		printf( "; Version:        %d" "\n", version );
	}
	
	int Main( int argc, char** argv )
	{
		if ( argc > 1 )
		{
			p7::dup2( p7::open( argv[1], p7::o_rdonly ), p7::stdin_fileno );
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

