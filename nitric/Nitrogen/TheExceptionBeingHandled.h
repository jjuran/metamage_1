// TheExceptionBeingHandled.h

#ifndef NITROGEN_THEEXCEPTIONBEINGHANDLED_H
#define NITROGEN_THEEXCEPTIONBEINGHANDLED_H

#ifndef NITROGEN_CONVERT_H
#include "Nitrogen/Convert.h"
#endif

#include <list>

namespace Nitrogen
  {
   class TheExceptionBeingHandled
     {
      private:
         // not implemented:
         TheExceptionBeingHandled( const TheExceptionBeingHandled& );
         TheExceptionBeingHandled& operator=( const TheExceptionBeingHandled& );
         void operator&() const;
         
      public:
         TheExceptionBeingHandled()  {}
     };


   template < class Output >
   class ExceptionConversionRegistrationLink;
   
   
   template < class Output >
   class ExceptionConverter
     {
      friend class ExceptionConversionRegistrationLink< Output >;
      
      public:
         typedef Output (*ConversionFunction)( const TheExceptionBeingHandled& );
         
      private:
         typedef std::list< ConversionFunction > List;
         
         List list;

         // not implemented:
            ExceptionConverter( const ExceptionConverter& );
            ExceptionConverter& operator=( const ExceptionConverter& );
                  
      public:
         ExceptionConverter()
           {}

         Output Convert( const TheExceptionBeingHandled& exception ) const
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
         
         Output Convert( const TheExceptionBeingHandled& exception, Output defaultValue ) const
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
   Output ConvertTheExceptionBeingHandled( const TheExceptionBeingHandled& )
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

   template < class Output,
              class Exception,
              Output (*convert)( const Exception& ) = static_cast< Output(*)( const Exception& ) >( Convert ) >
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
              Output (*convert)( const TheExceptionBeingHandled& ) >
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
   void RegisterExceptionConversion()
     {
      typedef Output (*ConversionFunction)( const Exception& );
      RegisterExceptionConversion< Output, Exception, static_cast<ConversionFunction>( Convert ) >();
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
         
      Output operator()( const TheExceptionBeingHandled& ) const
        {
         return hasDefaultValue
                  ? TheGlobalExceptionConverter< Output >().Convert( TheExceptionBeingHandled(), defaultValue ) 
                  : TheGlobalExceptionConverter< Output >().Convert( TheExceptionBeingHandled() ); 
        }
     };
  }

#endif
