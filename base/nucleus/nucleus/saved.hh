// nucleus/saved.hh
// ----------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2005-2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_SAVED_HH
#define NUCLEUS_SAVED_HH


namespace nucleus
{
	
	template < class Details >
	struct swap_with_saved
	{
		static typename Details::value_type apply( Details                       details,
		                                           typename Details::param_type  param )
		{
			typename Details::value_type previous = details.get();
			
			details.set( param );
			
			return previous;
		}
	};
	
	template < class Details >
	class saved
	{
		public:
			typedef typename Details::value_type  value_type;
			typedef typename Details::param_type  param_type;
			
			typedef swap_with_saved< Details > swapper;
		
		private:
			Details     details;
			value_type  value;
			
			// Non-copyable
			saved           ( const saved& );
			saved& operator=( const saved& );
		
		public:
			saved(           ) : details(   ), value( details.get() )  {}
			saved( Details d ) : details( d ), value( details.get() )  {}
			
			saved( param_type param ) : details(), value( swap( param ) )
			{
			}
			
			~saved()  { restore(); }
			
			void restore()
			{
				set( value );
			}
			
			void set( param_type param )
			{
				details.set( param );
			}
			
			value_type swap( param_type param )
			{
				return swapper::apply( details, param );
			}
	};
	
}

#endif

