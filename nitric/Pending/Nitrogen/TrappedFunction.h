/*	=================
 *	TrappedFunction.h
 *	=================
 */

#ifndef NITROGEN_TRAPPEDFUNCTION_H
#define NITROGEN_TRAPPEDFUNCTION_H

// Nitrogen
#ifndef NITROGEN_POINTERTOFUNCTION_H
#include "Nitrogen/PointerToFunction.h"
#endif


namespace Nitrogen
{
	
	template < class Exception, class Closure >
	class ExceptionTrap : public ConstantFunction< typename Closure::result_type >
	{
		private:
			Closure closure;
		
		public:
			ExceptionTrap( const Closure& closure ) : closure( closure )  {}
			
			typename Closure::result_type operator()() const
			{
				try
				{
					throw;
				}
				catch ( Exception )  {}
				
				return closure();
			}
	};
	
	template < class Exception, class Closure >
	ExceptionTrap< Exception, Closure > TrapException( Exception, const Closure& closure )
	{
		return ExceptionTrap< Exception, Closure >( closure );
	}
	
	template < class F, class Trap >
	class TrappedUnaryFunction : public std::unary_function< typename F::argument_type, typename F::result_type >
	{
		private:
			F f;
			Trap trap;
		
		public:
			TrappedUnaryFunction( F f, Trap trap ) : f( f ), trap( trap )  {}
			
			typename F::result_type operator()( const typename F::argument_type& arg ) const
			{
				try
				{
					return f( arg );
				}
				catch ( ... )
				{
					return trap();
				}
			}
	};
	
	template < class Trap, class F >
	TrappedUnaryFunction< F, Trap > Trap1( const F& f,
	                                       const Trap& trap )
	{
		return TrappedUnaryFunction< F, Trap >( f, trap );
	}
	
}

#endif

