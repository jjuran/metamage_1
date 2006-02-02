/*	============
 *	TextInput.hh
 *	============
 */

#pragma once

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
	
	// Object must have a default constructor (or we could use exceptions).
	template < class Object >
	class ObjectPipe
	{
		private:
			std::list< Object > objects;
		
		public:
			bool Empty() const  { return objects.empty(); }
			bool Ready() const  { return !Empty();        }
			
			int Write( const Object& data )
			{
				objects.push_back( data );
				return data.size();
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
	
	class TextInputAdapter
	{
		private:
			IOHandle input;             // The raw input stream
			StringPipe myStrings;       // A queue for storing lines of text
			std::string myPartialData;  // Storage for the current line of text
			char myLastNewlineChar;     // Used for automatic newline recognition
			bool ended;                 // Indicates end-of-file.
			
			bool GetMore();             // Called when the string pipe is empty.
		
		public:
			TextInputAdapter()  {}
			
			TextInputAdapter( const IOHandle& input, std::size_t bufferSize = 512 ) 
			:
				input            ( input ),
				myLastNewlineChar( '\0'  ),
				ended            ( false )
			{}
			
			bool Ended() const  { return ended; }
			bool Ready()  { return myStrings.Ready()  ||  GetMore() && Ready(); }
			
			std::string Read()
			{
				Ready();
				return myStrings.Read();
			}
	};
	
}

#endif

