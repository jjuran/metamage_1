/*
	region_scanner.hh
	-----------------
*/

#ifndef QUICKDRAW_REGIONSCANNER_HH
#define QUICKDRAW_REGIONSCANNER_HH


namespace quickdraw
{
	
	class region_scanner
	{
		private:
			short*           its_mark;
			unsigned short*  its_temp;
			const unsigned   its_line_size;
		
		public:
			region_scanner( short* out, unsigned short* temp, unsigned size );
			
			bool scan( short h,
			           short v,
			           const unsigned short* bits,
			           const unsigned short* prev,
			           int                   margin = 0 );
			
			void finish( short                  h,
			             short                  v,
			             const unsigned short*  prev,
			             int                    margin = 0 );
	};
	
}

#endif
