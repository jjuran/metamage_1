// Str.h

#ifndef NITROGEN_STR_H
#define NITROGEN_STR_H

#ifndef NITROGEN_CONVERT_H
#include "Nitrogen/Convert.h"
#endif

#include <string>

namespace Nitrogen
  {
   class StringTooLong {};
   
   void CopyToPascalString( const unsigned char *source,
                            unsigned char *destination,
                            unsigned char destinationLength );

   void CopyToPascalString( const char *source,
                            unsigned char *destination,
                            unsigned char destinationLength );

   void CopyToPascalString( const std::string& source,
                            unsigned char *destination,
                            unsigned char destinationLength );
                            
   template< unsigned char length >
   class Str
     {
      private:
         unsigned char string[length + 1];
      
      public:
         Str()                                                    { string[0] = 0; }
         Str( const unsigned char* s )                            { CopyToPascalString( s, string, length ); }
         Str( const char* s )                                     { CopyToPascalString( s, string, length ); }
         Str( const std::string& s )                              { CopyToPascalString( s, string, length ); }
         
         Str& operator=( const unsigned char* s )                 { CopyToPascalString( s, string, length ); return *this; } 
         Str& operator=( const char* s )                          { CopyToPascalString( s, string, length ); return *this; } 
         Str& operator=( const std::string& s )                   { CopyToPascalString( s, string, length ); return *this; } 
          
         unsigned char& operator[]( unsigned char i )             { return string[i]; }
         const unsigned char& operator[]( unsigned char i ) const { return string[i]; }
      
         operator unsigned char*()                                { return string; }
         operator const unsigned char *() const                   { return string; }
     };
   
   typedef Str<255> Str255;
   typedef Str<63>  Str63;
   typedef Str<32>  Str32;
   typedef Str<31>  Str31;
   typedef Str<27>  Str27;
   typedef Str<15>  Str15;
   
   template <>
   struct Converter< std::string, const unsigned char * >: public std::unary_function< const unsigned char *, std::string >
     {
      std::string operator()( const unsigned char *input ) const
        {
         const char *begin = reinterpret_cast<const char *>( input + 1 );
         return std::string( begin, begin+input[0] ); 
        }
     };
   
   /*
   template <>
   struct Converter< std::string, unsigned char * >: public std::unary_function< unsigned char *, std::string >
     {
      std::string operator()( unsigned char *input ) const
        {
         return Converter< std::string, const unsigned char * >()( input );
        }
     };
   */
	
	template <>
	struct ConvertInputTraits< unsigned char* > : ConvertInputTraits< const unsigned char* > {};
	
	template < unsigned char length >
	struct ConvertInputTraits< const unsigned char[length] > : ConvertInputTraits< const unsigned char* > {};
	
	template < unsigned char length >
	struct ConvertInputTraits<       unsigned char[length] > : ConvertInputTraits< const unsigned char* > {};
	
   template < unsigned char length >
   struct Converter< std::string, Str<length> >: public std::unary_function< Str<length>, std::string >
     {
      std::string operator()( const Str<length>& input ) const
        {
         return Converter< std::string, const unsigned char * >()( input );
        }
     };
  }

#endif
