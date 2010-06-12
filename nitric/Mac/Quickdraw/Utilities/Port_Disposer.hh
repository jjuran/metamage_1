/*
	Mac/Quickdraw/Utilities/Port_Disposer.hh
	----------------------------------------
*/

#ifndef MAC_QUICKDRAW_UTILITIES_PORTDISPOSER_HH
#define MAC_QUICKDRAW_UTILITIES_PORTDISPOSER_HH


namespace Mac
{
	
	template < class Port >
	class Port_Disposer
	{
		typedef Port  argument_type;
		typedef void  result_type;
		
		private:
			typedef pascal void (*Dispose)( Port );
			
			Dispose dispose;
		
		public:
			Port_Disposer() : dispose()
			{
			}
			
			Port_Disposer( Dispose d ) : dispose( d )
			{
			}
			
			void operator()( Port port ) const
			{
				if ( dispose )
				{
					dispose( port );
				}
			}
	};
	
}

#endif

