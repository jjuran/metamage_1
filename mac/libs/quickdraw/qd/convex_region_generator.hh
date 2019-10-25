/*
	convex_region_generator.hh
	--------------------------
*/

#ifndef QUICKDRAW_CONVEXREGIONGENERATOR_HH
#define QUICKDRAW_CONVEXREGIONGENERATOR_HH


namespace quickdraw
{
	
	class convex_region_generator
	{
		private:
			short*  its_mark;
			short   its_prev_left;
			short   its_prev_right;
		
		public:
			convex_region_generator( short* out );
			
			void start( short v, short left, short right );
			
			void extend_left ( short v, short left  );            /*  /  |  */
			void extend_right( short v, short right );            /*  |  \  */
			
			void condense_left ( short v, short right );          /*  |  /  */
			void condense_right( short v, short left  );          /*  \  |  */
			
			void tack_left ( short v, short left, short right );  /*  /  /  */
			void tack_right( short v, short left, short right );  /*  \  \  */
			
			void move_left ( short v, short left, short right );  /*  ?  ?  */
			void move_right( short v, short left, short right );  /*  ?  ?  */
			
			void expand  ( short v, short left, short right );    /*  /  \  */
			void contract( short v, short left, short right );    /*  \  /  */
			
			short* finish( short v );
			
			short* finish_reflected( short v, short* begin );
	};
	
}

#endif
