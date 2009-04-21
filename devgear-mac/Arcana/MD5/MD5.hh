/*	======
 *	MD5.hh
 *	======
 */

#ifndef MD5_HH
#define MD5_HH


namespace MD5
{
	
	typedef unsigned int Word;
	typedef unsigned long long BitCount;
	
	struct Result
	{
		unsigned char data[ 16 ];
	};
	
	Result Digest_Bits( const void* input, const BitCount& bits );
	
	inline Result Digest_Bytes( const void* input, std::size_t bytes )
	{
		return Digest_Bits( input, BitCount( bytes ) * 8 );
	}
	
	struct Buffer
	{
		Buffer();
		Word a, b, c, d;
	};
	
	class Engine
	{
		private:
			int blockCount;
			Buffer state;
		
		public:
			Engine() : blockCount( 0 )  {}
			void DoBlock( const void* input );  // 64 bytes
			void Finish( const void* input, int bitCount );
			const Result& GetResult();
	};
	
}

#endif

