/*	=================
 *	Vectoria/Units.hh
 *	=================
 */

#ifndef VECTORIA_UNITS_HH
#define VECTORIA_UNITS_HH

#include "Vectoria/Trig.hh"


namespace Vectoria
{
	
	struct Identity_Conversion
	{
		template < class Numeric >  static Numeric Convert  ( Numeric value )  { return value; }
		template < class Numeric >  static Numeric Normalize( Numeric value )  { return value; }
	};
	
	
	template < class Type > struct Category_Traits;
	
	
	template < class Category, class Conversion >
	class Units
	{
		private:
			double myValue;
		
		public:
			typedef Units< Category, Conversion > This;
			
			static double Convert  ( double value )  { return Conversion::Convert  ( value ); }
			static double Normalize( double value )  { return Conversion::Normalize( value ); }
			
			Units()  {}
			
			explicit Units( double value ) : myValue( value )  {}
			
			Units( const Units< Category, Identity_Conversion >& other ) : myValue( Convert( other.NormalizedValue() ) )  {}
			
			template < class OtherUnits >
			Units( const Units< Category, OtherUnits          >& other ) : myValue( Convert( other.NormalizedValue() ) )  {}
			
			double Value() const  { return myValue; }
			
			double NormalizedValue() const  { return Normalize( myValue ); }
			
			operator double() const  { return NormalizedValue(); }
			
			This& operator+=( const This& other )
			{
				myValue += other.myValue;
				
				return *this;
			}
			
			This& operator-=( const This& other )
			{
				myValue -= other.myValue;
				
				return *this;
			}
			
			This& operator*=( double factor )
			{
				myValue *= factor;
				
				return *this;
			}
			
			This& operator/=( double divisor )
			{
				myValue /= divisor;
				
				return *this;
			}
	};
	
	template < class Category, class Conversion >
	inline double Value( const Units< Category, Conversion >& val )
	{
		return val.Value();
	}
	
	template < class Category, class Conversion >
	inline Units< Category, Identity_Conversion > Normal( const Units< Category, Conversion >& val )
	{
		return Units< Category, Identity_Conversion >( val.NormalizedValue() );
	}
	
	// Negate
	template < class Category, class Conversion >
	inline Units< Category, Conversion > operator-( const Units< Category, Conversion >& x )
	{
		typedef Units< Category, Conversion > Unit;
		
		return Unit( -x.Value() );
	}
	
	// Add two values in the same units
	template < class Category, class Conversion >
	inline Units< Category, Conversion > operator+( const Units< Category, Conversion >&  one,
	                                                const Units< Category, Conversion >&  other )
	{
		typedef Units< Category, Conversion > Unit;
		
		return Unit( one.Value() + other.Value() );
	}
	
	// Subtract one value from another in the same units
	template < class Category, class Conversion >
	inline Units< Category, Conversion > operator-( const Units< Category, Conversion >&  one,
	                                                const Units< Category, Conversion >&  other )
	{
		typedef Units< Category, Conversion > Unit;
		
		return Unit( one.Value() - other.Value() );
	}
	
	// Multiply a value by a coefficient factor
	template < class Category, class Conversion >
	inline Units< Category, Conversion > operator*( const Units< Category, Conversion >&  one,
	                                                double                                factor )
	{
		typedef Units< Category, Conversion > Unit;
		
		return Unit( one.Value() * factor );
	}
	
	// Divide a value by a coefficient divisor
	template < class Category, class Conversion >
	inline Units< Category, Conversion > operator/( const Units< Category, Conversion >&  one,
	                                                double                                divisor )
	{
		typedef Units< Category, Conversion > Unit;
		
		return Unit( one.Value() / divisor );
	}
	
	// Divide a value by another in the same units
	template < class Category, class Conversion >
	inline double operator/( const Units< Category, Conversion >&  one,
	                         const Units< Category, Conversion >&  other )
	{
		return one.Value() / other.Value();
	}
	
	
	template < int i >
	struct Int
	{
		static double X()  { return i; }
	};
	
	
	template < class Conversion >
	struct Inverse
	{
		static double Convert  ( double value )  { return Conversion::Normalize( value ); }
		static double Normalize( double value )  { return Conversion::Convert  ( value ); }
	};
	
	
	template < class A, class B >
	struct Stack
	{
		static double Convert  ( double value )  { return B::Convert  ( A::Convert  ( value ) ); }
		static double Normalize( double value )  { return A::Normalize( B::Normalize( value ) ); }
	};
	
	template < class A, class B >  struct Rate : public Stack< A, Inverse< B > > {};
	
	
	template < class Coefficient, class Base_Conversion = Identity_Conversion >
	struct Factor
	{
		static double Convert( double value )
		{
			return Base_Conversion::Convert( value ) / Coefficient::X();
		}
		
		static double Normalize( double value )
		{
			return Base_Conversion::Normalize( value * Coefficient::X() );
		}
	};
	
	template < class Coefficient, class Base_Conversion = Identity_Conversion >
	struct Divisor : public Inverse< Factor< Coefficient, Inverse< Base_Conversion > > >  {};
	
	
	typedef Divisor< Int<  100 > >  Centi_Conversion;
	typedef Divisor< Int< 1000 > >  Milli_Conversion;
	
	typedef Divisor< Int< 1000 >, Milli_Conversion >  Micro_Conversion;
	typedef Divisor< Int< 1000 >, Micro_Conversion >  Nano_Conversion;
	typedef Divisor< Int< 1000 >, Nano_Conversion  >  Pico_Conversion;
	
	typedef Factor < Int< 1000 > >  Kilo_Conversion;
	
	typedef Factor < Int< 1000 >, Kilo_Conversion >  Mega_Conversion;
	typedef Factor < Int< 1000 >, Mega_Conversion >  Giga_Conversion;
	typedef Factor < Int< 1000 >, Giga_Conversion >  Tera_Conversion;
	
	/*
	struct Minutes_Tag : public InverseProportionalUnitTag< Int< 60 > >  {};
	struct Hours_Tag   : public InverseProportionalUnitTag< Int< 60 >, Minutes_Tag >  {};
	struct Days_Tag    : public InverseProportionalUnitTag< Int< 24 >, Hours_Tag   >  {};
	struct Weeks_Tag   : public InverseProportionalUnitTag< Int< 7  >, Days_Tag    >  {};
	
	struct MacTicks_Tag : public ProportionalUnitTag< Int< 60 > >  {};
	struct CDFrames_Tag : public ProportionalUnitTag< Int< 75 > >  {};
	*/
	
	typedef Identity_Conversion Kelvin_Conversion;
	
	struct Celsius_Conversion
	{
		static double Difference()  { return -273.15; }
		
		static double Convert  ( double value )  { return value + Difference(); }
		static double Normalize( double value )  { return value - Difference(); }
	};
	
	struct Fahrenheit_Conversion
	{
		static double Factor()      { return 9 / 5.0; }
		static double Difference()  { return 32;      }
		
		static double Convert( double value )
		{
			return Celsius_Conversion::Convert( value ) * Factor() + Difference();
		}
		
		static double Normalize( double value )
		{
			return Celsius_Conversion::Normalize( ( value - Difference() ) / Factor() );
		}
	};
	
	
	typedef Identity_Conversion Radians_Conversion;
	
	struct Pi2X { static double X()  { return pi * 2; }  };
	
	typedef Factor < Pi2X,       Radians_Conversion >  Circles_Conversion;
	typedef Divisor< Int< 360 >, Circles_Conversion >  Degrees_Conversion;
	
	
	struct ItemCount_Category    {};
	struct Proportion_Category   {};
	struct Space_Category        {};
	struct Time_Category         {};
	struct Angle_Category        {};
	struct Temperature_Category  {};
	
	
	typedef Units< Proportion_Category, Centi_Conversion > Percent;
	
	typedef Units< Space_Category, Identity_Conversion > Meters;
	typedef Units< Space_Category, Milli_Conversion    > Millimeters;
	typedef Units< Space_Category, Kilo_Conversion     > Kilometers;
	
	typedef Units< Time_Category, Identity_Conversion > Seconds;  // natural unit
	typedef Units< Time_Category, Factor< Int< 60 > > > Minutes;
	
	typedef Units< Angle_Category, Radians_Conversion > Radians;
	typedef Units< Angle_Category, Degrees_Conversion > Degrees;
	
	// typedef Units< Per< Meters, Seconds > > MetersPerSecond;
	
	
	template < class Numerator, class Denominator >
	struct Per
	{
		
	};
	
}

#endif

