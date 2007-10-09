/*	============
 *	TextInput.hh
 *	============
 */

#ifndef IO_TEXTINPUT_HH
#define IO_TEXTINPUT_HH

// Standard C++
#include <list>
#include <string>

// Io
#include "io/io.hh"

// Nucleus
#include "Nucleus/Convert.h"


namespace Io
{
	
	template < class Object >
	class ObjectPipe
	{
		private:
			std::list< Object > objects;
		
		public:
			bool Empty() const  { return objects.empty(); }
			bool Ready() const  { return !Empty();        }
			
			void Write( const Object& data )
			{
				objects.push_back( data );
			}
			
			Object Read()
			{
				if ( Empty() )  throw io::no_input_pending();
				
				Object obj = objects.front();
				objects.pop_front();
				
				return obj;
			}
	};
	
	typedef ObjectPipe< std::string > StringPipe;
	
	class TextInputBuffer
	{
		private:
			StringPipe   itsStrings;              // A queue for storing lines of text
			std::string  itsPartialData;          // Storage for the current line of text
			char         itsLastNewlineChar;      // Used for automatic newline recognition
			bool         itHasReachedEndOfInput;  // Indicates end-of-file.
			
			void WritePartialData();
		
		public:
			TextInputBuffer() : itsLastNewlineChar    ( '\0'  ),
			                    itHasReachedEndOfInput( false )  {}
			
			void ReceiveBlock( const char* data, std::size_t bytes );
			
			bool Ended() const  { return itHasReachedEndOfInput; }
			bool Ready() const  { return itsStrings.Ready(); }
			
			std::string Read()  { return itsStrings.Read(); }
			
			bool ReceiveEndOfInput();
	};
	
	template < class Input >
	class TextInputAdapter
	{
		private:
			Input            itsInput;   // The raw input stream
			TextInputBuffer  itsBuffer;
			
			bool GetMore();              // Called when the string pipe is empty.
		
		public:
			TextInputAdapter()  {}
			
			TextInputAdapter( Input input ) : itsInput( input )  {}
			
			Input& GetStream()  { return itsInput; }
			
			bool Ended()  { return !Ready()  &&  itsBuffer.Ended(); }
			bool Ready()  { return itsBuffer.Ready()  ||  GetMore() && Ready(); }
			
			std::string Read()
			{
				(void) Ready();
				
				return itsBuffer.Read();
			}
	};
	
	template < class Input >
	bool TextInputAdapter< Input >::GetMore()
	{
		const std::size_t blockSize = 4096;
		
		char data[ blockSize ];
		
		try
		{
			typedef typename Nucleus::ConvertInputTraits< Input >::ConverterInputType Stream;
			
			std::size_t bytes = io::read( Stream( itsInput ), data, blockSize );
			
			if ( bytes == 0 )
			{
				return itsBuffer.ReceiveEndOfInput();
			}
			
			itsBuffer.ReceiveBlock( data, bytes );
			
			return true;
		}
		catch ( const io::no_input_pending& ) {}
		catch ( const io::end_of_input&     )
		{
			return itsBuffer.ReceiveEndOfInput();
		}
		
		return false;
	}
	
}

#endif

