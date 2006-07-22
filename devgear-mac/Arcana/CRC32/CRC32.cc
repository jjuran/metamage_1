/*	========
 *	CRC32.cc
 *	========
 */

// A recoded implementation of Algorithm Three from "Fast CRC32 in Software".
// See <http://www.cl.cam.ac.uk/Research/SRG/bluebook/21/crc/node6.html>.


#include "CRC32.hh"


namespace CRC32
{
	
	class Table
	{
		private:
			unsigned int data[ 256 ];
		
		public:
			Table();
			
			unsigned int operator[]( unsigned char i ) const  { return data[ i ]; }
	};
	
	enum
	{
		kQuotient = 0x04c11db7
	};
	
	Table::Table()
	{
		for ( int i = 0;  i < 256;	++i )
		{
			unsigned int crc = i << 24;
			
			for ( int j = 0;  j < 8;  ++j )
			{
				if ( crc & 0x80000000 )
				{
					crc = (crc << 1) ^ kQuotient;
				}
				else
				{
					crc = crc << 1;
				}
			}
			
			data[ i ] = crc;
		}
	}
	
	static Table gTable;
	
	unsigned int Checksum( const void* text, unsigned int bytes )
	{
		if ( bytes <= 4 )
		{
			return *reinterpret_cast< const unsigned int* >( text ) >> ((4 - bytes) * 8);
		}
		
		const unsigned char* data = reinterpret_cast< const unsigned char* >( text );
		const unsigned char* end = data + bytes;
		
		unsigned int result = data[ 0 ] << 24
		                    | data[ 1 ] << 16
		                    | data[ 2 ] << 8
		                    | data[ 3 ];
		
		result = ~result;
		data += 4;
		
		while ( data < end )
		{
			result = (result << 8 | *data++) ^ gTable[ result >> 24 ];
		}
		
		return ~result;
	}
	
}  // namespace CRC32

