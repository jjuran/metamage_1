/*
	iota/convertible_string.hh
	--------------------------
*/

#ifndef IOTA_CONVERTIBLESTRING_HH
#define IOTA_CONVERTIBLESTRING_HH


namespace iota
{
	
	class convertible_string
	{
		private:
			typedef unsigned char Byte;
			
			Byte buffer[ 257 ];
	
		public:
			convertible_string()
			{
				clear();
			}
			
			Byte* get()  { return buffer; }
			
			/*
				Defining both of these pointer conversions
				prevents use in boolean context, as intended.
			*/
			
			operator Byte*()  { return get(); }
			operator void*()  { return get(); }
			
			Byte size() const  { return buffer[ 0 ]; }
			
			const char* c_str()
			{
				Byte len = buffer[ 0 ];
				
				buffer[ 1 + len ] = '\0';
				
				return (const char*) buffer + 1;
			}
			
			void clear()  { buffer[ 0 ] = '\0'; }
	};
	
}

#endif
