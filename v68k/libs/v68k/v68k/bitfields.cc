/*
	bitfields.cc
	------------
*/

#include "v68k/bitfields.hh"

// v68k
#include "v68k/state.hh"


#ifdef __MWERKS__
#pragma exceptions off
#endif

namespace v68k
{

#define N( x )  (!!(x) << 3)
#define Z( x )  (!!(x) << 2)


op_result normalize_bitfield_operands( processor_state& s, op_params& pb )
{
	int16_t width  = pb.first      & 0x3f;
	int32_t offset = pb.first >> 6 & 0x3f;
	
	if ( width & (1 << 5) )
	{
		width = s.d(width & 0x7) & 0x1f;
	}
	
	if ( width == 0 )
	{
		width = 32;
	}
	
	if ( offset & (1 << 5) )
	{
		offset = s.d(offset & 0x7);
	}
	
	pb.result = offset;
	
	if ( (int32_t) pb.target < 0 )
	{
		pb.address += offset >> 3;
		
		offset &= 0x7;
	}
	
	offset &= 0x1f;
	
	uint32_t extent = (offset + width) & 0x1f;
	
	if ( (int32_t) pb.target >= 0 )
	{
		uint32_t bits_mask = 2u * (1 << (width - 1)) - 1;
		
		bits_mask = bits_mask >> extent | bits_mask << (32 - extent);
		
		pb.address = bits_mask;
	}
	
	pb.second = pb.first >> 12;  // src/dst data register
	pb.first  = extent   << 16 | width;
	
	return Ok;
}


class bitfield
{
	private:
		uint32_t addr;
		uint16_t offset;
		int16_t  width;  // width can go negative at the end
		uint8_t  sign_mask;
		
		struct boolean {};
	
	public:
		bitfield( const op_params& pb );
		
		operator boolean*() const  { return (boolean*) (width > 0); }
		
		uint8_t get_sign_mask() const  { return sign_mask; }
		
		uint32_t get_addr() const  { return addr; }
		uint8_t  get_mask() const;
		
		bitfield& operator++();
};

bitfield::bitfield( const op_params& pb )
{
	addr = pb.address;
	
	width  = pb.first;
	offset = pb.result & 0x07;
	
	sign_mask = 1 << (7 - offset);
}

uint8_t bitfield::get_mask() const
{
	uint8_t mask = (sign_mask << 1) - 1;
	
	if ( offset + width < 8 )
	{
		uint8_t end_mask = ~((1 << (8 - (offset + width))) - 1);
		
		mask &= end_mask;
	}
	
	return mask;
}

bitfield& bitfield::operator++()
{
	++addr;
	width -= 8 - offset;
	offset = 0;
	sign_mask = 0;
	
	return *this;
}


static
op_result set_CCR_for_BFINS( processor_state& s, op_params& pb )
{
	const uint16_t width = pb.first;
	
	const uint32_t sign_mask = 1 << (width - 1);
	const uint32_t data_mask = 2 * sign_mask - 1;
	
	uint32_t Ds = s.d(pb.second);
	
	s.sr.nzvc = N(  Ds & sign_mask       )
	          | Z( (Ds & data_mask) == 0 );
	
	return Ok;
}

static
op_result sign_extend_BFEXTS( processor_state& s, op_params& pb )
{
	const uint16_t width = pb.first;
	
	const uint32_t sign_mask = 1 << (width - 1);
	const uint32_t data_mask = 2 * sign_mask - 1;
	
	uint32_t& Dr = s.d(pb.second);
	
	if ( Dr & sign_mask )
	{
		Dr |= ~data_mask;
	}
	
	return Ok;
}

op_result microcode_BFTST_Dn( processor_state& s, op_params& pb )
{
	const uint32_t offset = pb.result & 0x3f;
	
	const uint32_t sign_mask = 1 << (31 - offset);
	const uint32_t bits_mask = pb.address;
	
	const uint32_t  n = pb.target;
	const uint32_t Dn = s.d(n);
	
	s.sr.nzvc = N(  Dn & sign_mask       )
	          | Z( (Dn & bits_mask) == 0 );
	
	return Ok;
}

op_result microcode_BFCHG_Dn( processor_state& s, op_params& pb )
{
	microcode_BFTST_Dn( s, pb );
	
	const uint32_t mask = pb.address;
	
	uint32_t   n = pb.target;
	uint32_t& Dn = s.d(n);
	
	Dn ^= mask;
	
	return Ok;
}

op_result microcode_BFCLR_Dn( processor_state& s, op_params& pb )
{
	microcode_BFTST_Dn( s, pb );
	
	const uint32_t mask = pb.address;
	
	uint32_t   n = pb.target;
	uint32_t& Dn = s.d(n);
	
	Dn &= ~mask;
	
	return Ok;
}

op_result microcode_BFSET_Dn( processor_state& s, op_params& pb )
{
	microcode_BFTST_Dn( s, pb );
	
	const uint32_t mask = pb.address;
	
	uint32_t   n = pb.target;
	uint32_t& Dn = s.d(n);
	
	Dn |= mask;
	
	return Ok;
}

op_result microcode_BFINS_Dn( processor_state& s, op_params& pb )
{
	const uint32_t extent = pb.first >> 16;
	const uint32_t mask   = pb.address;
	
	const uint32_t Dsrc = s.d(pb.second);
	const uint32_t data = Dsrc >> extent | Dsrc << (32 - extent);
	
	uint32_t   n = pb.target;
	uint32_t& Dn = s.d(n);
	
	Dn &= ~mask;
	Dn |= mask & data;
	
	return set_CCR_for_BFINS( s, pb );
}

op_result microcode_BFFFO_Dn( processor_state& s, op_params& pb )
{
	microcode_BFEXTU_Dn( s, pb );
	
	const uint32_t width = pb.first & 0x3f;
	
	uint32_t& Ddst = s.d(pb.second);
	
	const uint32_t data = Ddst;
	
	int i;
	
	for ( i = width - 1;  i >= 0;  --i )
	{
		if ( data & (1 << i) )
		{
			break;
		}
	}
	
	Ddst = pb.result + (width - 1 - i);
	
	return Ok;
}

op_result microcode_BFEXTU_Dn( processor_state& s, op_params& pb )
{
	microcode_BFTST_Dn( s, pb );
	
	const uint32_t extent = pb.first >> 16;
	const uint32_t mask   = pb.address;
	
	const uint32_t  n = pb.target;
	const uint32_t Dn = s.d(n);
	
	const uint32_t data = Dn & mask;
	
	uint32_t& Ddst = s.d(pb.second);
	
	Ddst = data << extent | data >> (32 - extent);
	
	return Ok;
}

op_result microcode_BFEXTS_Dn( processor_state& s, op_params& pb )
{
	microcode_BFEXTU_Dn( s, pb );
	
	return sign_extend_BFEXTS( s, pb );
}

op_result microcode_BFTST( processor_state& s, op_params& pb )
{
	const function_code_t fc = s.data_space();
	
	bitfield bf = pb;
	
	uint8_t data;
	
	if ( ! s.get_byte( bf.get_addr(), data, fc ) )
	{
		return Bus_error;
	}
	
	uint8_t sign_data = data & bf.get_sign_mask();
	uint8_t zero_data = data & bf.get_mask();
	
	while ( ++bf )
	{
		if ( ! s.get_byte( bf.get_addr(), data, fc ) )
		{
			return Bus_error;
		}
		
		zero_data |= data & bf.get_mask();
	}
	
	s.sr.nzvc = N( sign_data != 0 )
	          | Z( zero_data == 0 );
	
	return Ok;
}

op_result microcode_BFCHG( processor_state& s, op_params& pb )
{
	if ( op_result result = microcode_BFTST( s, pb ) )
	{
		return result;
	}
	
	const function_code_t fc = s.data_space();
	
	bitfield bf = pb;
	
	do
	{
		uint32_t addr = bf.get_addr();
		
		uint8_t data;
		
		s.get_byte( addr, data, fc );
		
		data ^= bf.get_mask();
		
		if ( ! s.put_byte( addr, data, fc ) )
		{
			return Bus_error;
		}
	}
	while ( ++bf );
	
	return Ok;
}

op_result microcode_BFCLR( processor_state& s, op_params& pb )
{
	if ( op_result result = microcode_BFTST( s, pb ) )
	{
		return result;
	}
	
	const function_code_t fc = s.data_space();
	
	bitfield bf = pb;
	
	do
	{
		const uint32_t addr = bf.get_addr();
		
		uint8_t data;
		
		s.get_byte( addr, data, fc );
		
		data &= ~bf.get_mask();
		
		if ( ! s.put_byte( addr, data, fc ) )
		{
			return Bus_error;
		}
	}
	while ( ++bf );
	
	return Ok;
}

op_result microcode_BFSET( processor_state& s, op_params& pb )
{
	if ( op_result result = microcode_BFTST( s, pb ) )
	{
		return result;
	}
	
	const function_code_t fc = s.data_space();
	
	bitfield bf = pb;
	
	do
	{
		const uint32_t addr = bf.get_addr();
		
		uint8_t data;
		
		s.get_byte( addr, data, fc );
		
		data |= bf.get_mask();
		
		if ( ! s.put_byte( addr, data, fc ) )
		{
			return Bus_error;
		}
	}
	while ( ++bf );
	
	return Ok;
}

op_result microcode_BFINS( processor_state& s, op_params& pb )
{
	if ( op_result result = microcode_BFTST( s, pb ) )
	{
		return result;
	}
	
	set_CCR_for_BFINS( s, pb );
	
	/*
		The offset will be in 0 .. 7.  Shifting right by that amount here
		(and subsequently left by 8 in the loop) is correct if we'll be
		using the entire source operand, but for shorter widths we have
		to skip some.
		
		Source value:
		 _______________________________________________________________
		|0|1|2|3|4|5|6|7|8|9|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|
		 ---------------------------------------------------------------
		
		In all cases, rotate source right by (offset + width) mod 32 bits
		
		Offset=0, width=32:  same (no rotation)
		
		Offset=0, width=29
		 _______________ _______________ _______________ _______________
		|3|4|5|6|7|8|9|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|-|-|-|
		 ---------------^---------------^---------------^---------------
		(rotate source right by 29 bits)
		
		Offset=3, width=29:
		 _______________ _______________ _______________ _______________
		|-|-|-|3|4|5|6|7|8|9|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|
		 ---------------^---------------^---------------^---------------
		(no rotation)
		
		Offset=3, width=27:
		 _______________ _______________ _______________ _______________
		|-|-|-|5|6|7|8|9|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|-|-|
		 ---------------^---------------^---------------^---------------
		(rotate source right by 30 bits)
		
		Offset=3, width=31:
		 _______________ _______________ _______________ _______________ _______________
		|-|-|-|1|2|3|4|5|6|7|8|9|a|b|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|-|-|-|-|-|-|
		 ---------------^---------------^---------------^---------------^---------------
		(rotate source right by 2 bits)
		
		Offset=3, width=20:
		 _______________ _______________ _______________
		|-|-|-|c|d|e|f|g|h|i|j|k|l|m|n|o|p|q|r|s|t|u|v|-|
		 ---------------^---------------^---------------
		(rotate source right by 23 bits)
		
	*/
	
	const function_code_t fc = s.data_space();
	
	const uint32_t extent = pb.first >> 16;
	
	bitfield bf = pb;
	
	uint32_t Ds = s.d(pb.second);
	
	Ds = Ds >> extent | Ds << (32 - extent);
	
	do
	{
		Ds = Ds << 8 | Ds >> 24;
		
		const uint8_t  mask = bf.get_mask();
		const uint32_t addr = bf.get_addr();
		
		uint8_t data;
		
		s.get_byte( addr, data, fc );
		
		data &= ~mask;
		data |= Ds & mask;
		
		if ( ! s.put_byte( addr, data, fc ) )
		{
			return Bus_error;
		}
	}
	while ( ++bf );
	
	return Ok;
}

op_result microcode_BFFFO( processor_state& s, op_params& pb )
{
	if ( op_result result = microcode_BFEXTU( s, pb ) )
	{
		return result;
	}
	
	const uint16_t width = pb.first;
	
	uint32_t& Dr = s.d(pb.second);
	
	const uint32_t data = Dr;
	
	int i;
	
	for ( i = width - 1;  i >= 0;  --i )
	{
		if ( data & (1 << i) )
		{
			break;
		}
	}
	
	Dr = pb.result + (width - 1 - i);
	
	return Ok;
}

op_result microcode_BFEXTU( processor_state& s, op_params& pb )
{
	if ( op_result result = microcode_BFTST( s, pb ) )
	{
		return result;
	}
	
	const function_code_t fc = s.data_space();
	
	const uint32_t extent = pb.first >> 16;
	
	uint32_t& Dr = s.d(pb.second);
	
	bitfield bf = pb;
	
	uint32_t result = 0;
	
	do
	{
		result = result << 8 | result >> 24;
		
		const uint32_t addr = bf.get_addr();
		const uint8_t  mask = bf.get_mask();
		
		uint8_t data;
		
		s.get_byte( addr, data, fc );
		
		data &= mask;
		
		result |= data;
		
	}
	while ( ++bf );
	
	result = result >> 8      | result << 24;
	result = result << extent | result >> (32 - extent);
	
	Dr = result;
	
	return Ok;
}

op_result microcode_BFEXTS( processor_state& s, op_params& pb )
{
	if ( op_result result = microcode_BFEXTU( s, pb ) )
	{
		return result;
	}
	
	return sign_extend_BFEXTS( s, pb );
}

}
