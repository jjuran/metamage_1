/*
	region_raster.hh
	----------------
*/

#ifndef QUICKDRAW_REGIONRASTER_HH
#define QUICKDRAW_REGIONRASTER_HH


namespace quickdraw
{
	
	class region_raster
	{
		private:
			const short*    its_next;
			unsigned char*  its_mask;
			const short     its_left;
			
			void invert_mask_bits( short begin, short end );
		
		public:
			static unsigned mask_size( const short* bbox );
			
			region_raster( const short*  bbox,
			               const short*  extent,
			               void*         buffer,
			               unsigned      size );
			
			short next_v() const  { return *its_next; }
			
			void load_mask( short v );
	};
	
}

#endif
