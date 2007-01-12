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
#include "Io/Exceptions.hh"
#include "Io/Handle.hh"


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
				if ( Empty() )  throw NoDataPending();
				
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
	
	class TextInputAdapter
	{
		private:
			IOHandle         input;   // The raw input stream
			TextInputBuffer  buffer;
			
			bool GetMore();             // Called when the string pipe is empty.
		
		public:
			TextInputAdapter()  {}
			
			TextInputAdapter( const IOHandle& input ) : input( input )  {}
			
			bool Ended() const  { return buffer.Ended(); }
			bool Ready()  { return buffer.Ready()  ||  GetMore() && Ready(); }
			
			std::string Read()
			{
				(void) Ready();
				
				return buffer.Read();
			}
	};
	
}

#endif

