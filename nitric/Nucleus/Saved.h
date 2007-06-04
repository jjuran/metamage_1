// Nucleus/Saved.h
// ---------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2005-2006 by Joshua Juran.
//
// This code was written entirely by the above contributor, who places it
// in the public domain.


#ifndef NUCLEUS_SAVED_H
#define NUCLEUS_SAVED_H


namespace Nucleus
{
	
	template < bool hasSwap >
	struct Swap_Traits
	{
		template < class Details >
		static typename Details::GetResult Swap( Details                         details,
		                                         typename Details::SetParameter  param )
		{
			return details.Swap( param );
		}
	};
	
	template <>
	struct Swap_Traits< false >
	{
		template < class Details >
		static typename Details::GetResult Swap( Details                         details,
		                                         typename Details::SetParameter  param )
		{
			typename Details::GetResult previous = details.Get();
			
			details.Set( param );
			
			return previous;
		}
	};
	
	template < class Details >
	class Saved
	{
		public:
			typedef typename Details::Value         Value;
			typedef typename Details::GetResult     GetResult;
			typedef typename Details::SetParameter  SetParameter;
			
			typedef Swap_Traits< Details::hasSwap > SwapTraits;
		
		private:
			Details  details;
			Value    value;
			
			Saved( const Saved& );  // Non-copyable
		
		public:
			Saved(           ) : details(   ), value( details.Get() )  {}
			Saved( Details d ) : details( d ), value( details.Get() )  {}
			
			Saved( SetParameter param ) : details(), value( Swap( param ) )  {}
			
			~Saved()  { Set( value ); }
			
			//const Value& Get() const  { return value; }
			
			void Set( SetParameter param )
			{
				details.Set( param );
			}
			
			void Restore()
			{
				details.Set( value );
			}
			
			GetResult Swap( SetParameter param )
			{
				return SwapTraits::Swap( details, param );
			}
	};
	
}

#endif

