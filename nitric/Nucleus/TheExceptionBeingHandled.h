// Nucleus/TheExceptionBeingHandled.h
// ----------------------------------
//
// Maintained by Joshua Juran

// Part of the Nitrogen project.
//
// Written 2002-2007 by Lisa Lippincott, Marshall Clow, and Joshua Juran.
//
// This code was written entirely by the above contributors, who place it
// in the public domain.


#ifndef NUCLEUS_THEEXCEPTIONBEINGHANDLED_H
#define NUCLEUS_THEEXCEPTIONBEINGHANDLED_H

#ifndef NUCLEUS_CONVERT_H
#include "Nucleus/Convert.h"
#endif

#include <list>

namespace Nucleus
  {
   class TheExceptionBeingHandled {};


   template < class Output >
   class ExceptionConversionRegistrationLink;
   
   
   template < class Output >
   class ExceptionConverter
     {
      friend class ExceptionConversionRegistrationLink< Output >;
      
      public:
         typedef Output (*ConversionFunction)( TheExceptionBeingHandled );
         
      private:
         typedef std::list< ConversionFunction > List;
         
         List list;

         // not implemented:
            ExceptionConverter( const ExceptionConverter& );
            ExceptionConverter& operator=( const ExceptionConverter& );
                  
      public:
         ExceptionConverter()
           {}

         Output Convert( TheExceptionBeingHandled exception ) const
           {
            try
              {
               throw;
              }
            catch ( const Output& result )
              {
               return result;
              }
            catch ( ... )
              {}
            
            for ( typename List::const_iterator i = list.begin(); i != list.end(); ++i )
               try
                 {
                  return (*i)( exception );
                 }
               catch( ... )
                 {}

            throw;
           }
         
         Output Convert( TheExceptionBeingHandled exception, Output defaultValue ) const
           {
            try
              {
               return Convert( exception );
              }
            catch ( ... )
              {}
            
            return defaultValue;
           }
     };
   
   
   template < class Output >
   ExceptionConverter<Output>& TheGlobalExceptionConverter()
     {
      static ExceptionConverter< Output > theGlobalExceptionConverter;
      return theGlobalExceptionConverter;
     }
   
   
   template < class Output >
   class ExceptionConversionRegistrationLink
     {
      public:
         typedef ExceptionConverter<Output>              Converter;
         typedef typename Converter::ConversionFunction  ConversionFunction;
         
      private:
         typedef typename Converter::List                List;
         
         List& list;
         typename List::iterator registered;
         
         // not implemented:
            ExceptionConversionRegistrationLink( const ExceptionConversionRegistrationLink& );
            ExceptionConversionRegistrationLink& operator=( const ExceptionConversionRegistrationLink& );
         
      public:
         ExceptionConversionRegistrationLink( ConversionFunction conversionFunction, Converter& converter )
           : list( converter.list ),
             registered( list.insert( list.begin(), conversionFunction ) )
           {
           }

         ExceptionConversionRegistrationLink( ConversionFunction conversionFunction )
           : list( TheGlobalExceptionConverter<Output>().list ),
             registered( list.insert( list.begin(), conversionFunction ) )
           {
           }
         
         ~ExceptionConversionRegistrationLink()
           {
            list.erase( registered );
           }
     };


   template < class Output,
              class Exception,
              Output (*convert)( const Exception& ) >
   Output ConvertTheExceptionBeingHandled( TheExceptionBeingHandled )
     {
      try
        {
         throw;
        }
      catch ( const Exception& exception )
        {
         return convert( exception );
        }
      
      // not reached:
      throw;
     }
	
	template < class Output, class Exception >
	Output ConvertException( const Exception& e )
	{
		return Convert< Output, Exception >( e );
	}
	
   template < class Output,
              class Exception,
              Output (*convert)( const Exception& ) = static_cast< Output(*)( const Exception& ) >( ConvertException ) >
   class ExceptionConversionRegistration
     {
      private:
         typedef ExceptionConverter<Output> Converter;
         
         ExceptionConversionRegistrationLink<Output> link;
      
      public:
         ExceptionConversionRegistration( Converter& converter )
           : link( ConvertTheExceptionBeingHandled< Output, Exception, convert >, converter )
           {}

         ExceptionConversionRegistration()
           : link( ConvertTheExceptionBeingHandled< Output, Exception, convert >, TheGlobalExceptionConverter<Output>() )
           {}
     };

   template < class Output,
              Output (*convert)( TheExceptionBeingHandled ) >
   class ExceptionConversionRegistration< Output, TheExceptionBeingHandled, convert >
     {
      private:
         typedef ExceptionConverter<Output> Converter;
         
         ExceptionConversionRegistrationLink<Output> link;
      
      public:
         ExceptionConversionRegistration( Converter& converter )
           : link( convert, converter )
           {}

         ExceptionConversionRegistration()
           : link( convert, TheGlobalExceptionConverter<Output>() )
           {}
     };
   
   template < class Output,
              class Exception,
              Output (*convert)( const Exception& ) >
   void RegisterExceptionConversion()
     {
      static const ExceptionConversionRegistration< Output, Exception, convert > registration;
     }
   
   template < class Output, class Exception >
   inline void RegisterExceptionConversion()
     {
      typedef Output (*ConversionFunction)( const Exception& );
#ifdef __MWERKS__
      // gcc 4:  Nucleus::Convert cannot appear in a constant-expression
      RegisterExceptionConversion< Output, Exception, static_cast<ConversionFunction>( ConvertException ) >();
#else
      // CW Pro 6:  ambiguous access to overloaded function
      RegisterExceptionConversion< Output, Exception, ConvertException >();
#endif
     }
   
   
   template < class Output >
   class Converter< Output, TheExceptionBeingHandled >: public std::unary_function< TheExceptionBeingHandled, Output >
     {
      private:
         bool hasDefaultValue;
         Output defaultValue;
      
      public:
         Converter()
           : hasDefaultValue( false )
           {}
         
         Converter( const Output& theDefaultValue )
           : hasDefaultValue( true ),
             defaultValue( theDefaultValue )
           {}
         
      Output operator()( TheExceptionBeingHandled e ) const
        {
         return hasDefaultValue
                  ? TheGlobalExceptionConverter< Output >().Convert( e, defaultValue ) 
                  : TheGlobalExceptionConverter< Output >().Convert( e ); 
        }
     };
  }

#endif
