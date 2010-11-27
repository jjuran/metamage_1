/*	======================
 *	FSTree_new_console.cc
 *	======================
 */

#include "Genie/FS/FSTree_new_console.hh"

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif

// POSIX
#include <sys/ttycom.h>

// Iota
#include "iota/decimal.hh"
#include "iota/strings.hh"

// chars
#include "charsets/extended_ascii.hh"
#include "charsets/MacRoman.hh"
#include "conv/mac_utf8.hh"
#include "encoding/utf8.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/serialize.hh"

// Nitrogen
#include "Mac/Sound/Functions/SysBeep.hh"

#include "Nitrogen/Quickdraw.hh"

// Genie
#include "Genie/Devices.hh"
#include "Genie/FileDescriptor.hh"
#include "Genie/Process.hh"
#include "Genie/FS/FSTree_Directory.hh"
#include "Genie/FS/FSTree_Property.hh"
#include "Genie/FS/ResolvePathname.hh"
#include "Genie/FS/sys/port/ADDR.hh"
#include "Genie/FS/TextEdit.hh"
#include "Genie/FS/TextEdit_text.hh"
#include "Genie/FS/Views.hh"
#include "Genie/IO/DynamicGroup.hh"
#include "Genie/IO/Terminal.hh"
#include "Genie/IO/TTY.hh"
#include "Genie/IO/VirtualFile.hh"
#include "Genie/Utilities/simple_map.hh"


namespace Genie
{
	
	namespace N = Nitrogen;
	namespace Ped = Pedestal;
	
	
	static void RunShellCommand( const plus::string& command )
	{
		const char* argv[] = { "-sh", "-c", "", NULL };
		
		const char* envp[] = { "PATH=/bin:/sbin:/usr/bin:/usr/sbin", NULL };
		
		argv[2] = command.c_str();
		
		Process& parent = GetProcess( 1 );
		
		Process& process = parent.vfork();
		
		try
		{
			process.Exec( "/bin/sh", argv, envp );
		}
		catch ( ... )
		{
			process.Exit( 127 );
		}
		
		parent.ResumeAfterFork();
	}
	
	
	struct ConsoleParameters
	{
		boost::shared_ptr< IOHandle >  itsTerminal;
		std::size_t         itsStartOfInput;
		std::size_t         itsStartOfOutput;
		bool                itHasReceivedEOF;
		
		ConsoleParameters()
		:
			itsStartOfInput(),
			itsStartOfOutput(),
			itHasReceivedEOF()
		{
		}
	};
	
	typedef simple_map< const FSTree*, ConsoleParameters > ConsoleParametersMap;
	
	static ConsoleParametersMap gConsoleParametersMap;
	
	
	static bool Console_UserCommand_Hook( TextEdit& that, Ped::CommandCode code )
	{
		bool handled = false;
		
		const FSTree* key = that.GetKey();
		
		TextEditParameters& textParams = TextEditParameters::Get( key );
		
		ConsoleParameters& params = gConsoleParametersMap[ key ];
		
		if ( textParams.itsSelection.start < params.itsStartOfInput )
		{
			switch ( code )
			{
				// Edit
				case 'past':  // kHICommandPaste
				case 'pste':
					{
						TEHandle hTE = that.Get();
						
						ASSERT( hTE != NULL );
						
						TERec& te = **hTE;
						
						const short length = te.teLength;
						
						::TESetSelect( length, length, hTE );
						
						textParams.itsSelection.start =
						textParams.itsSelection.end   = length;
					}
					
					break;
				
				case 'cut ':
				case 'clea':
					Mac::SysBeep();
					
					handled = true;
					break;
				
				default:
					break;
			}
		}
		
		return handled;
	}
	
	
	static void Console_On_EnterKey( TextEditParameters& params )
	{
		const plus::string& s = params.its_mac_text;
		
		plus::string command( s.begin() + params.itsSelection.start,
		                      s.begin() + params.itsSelection.end );
		
		//command += '\n';
		
		RunShellCommand( plus::utf8_from_mac( command ) );
	}
	
	static void SendSignalToProcessGroupForKey( int signo, const FSTree* key )
	{
		ConsoleParameters& params = gConsoleParametersMap[ key ];
		
		const boost::shared_ptr< IOHandle >& handle = params.itsTerminal;
		
		TerminalHandle& terminal( IOHandle_Cast< TerminalHandle >( *handle ) );
		
		send_signal_to_foreground_process_group_of_terminal( signo, terminal );
	}
	
	static bool Try_Control_Character( TextEdit& that, const EventRecord& event )
	{
		const UInt32 kEitherControlKey = controlKey | rightControlKey;
		
		const char c = event.message & charCodeMask;
		
		if ( event.modifiers & kEitherControlKey  &&  c < 0x20 )
		{
			typedef const FSTree* Key;
			
			const Key key = that.GetKey();
			
			const TextEditParameters& params = TextEditParameters::Get( key );
			
			ConsoleParameters& consoleParams = gConsoleParametersMap[ key ];
			
			const char cntrl = c | 0x40;
			
			switch ( cntrl )
			{
				case 'A':
					that.Select( consoleParams.itsStartOfInput,
					             consoleParams.itsStartOfInput );
					break;
				
				case 'E':
					that.Select( params.its_mac_text.size(),
					             params.its_mac_text.size() );
					break;
				
				case 'C':
					SendSignalToProcessGroupForKey( SIGINT, key );
					break;
				
				case 'Z':
					SendSignalToProcessGroupForKey( SIGTSTP, key );
					break;
				
				case 'D':
					if ( params.its_mac_text.size() - consoleParams.itsStartOfInput <= 0 )
					{
						consoleParams.itHasReceivedEOF = true;
					}
					else
					{
						Mac::SysBeep();
					}
					
					break;
			}
			
			return true;
		}
		
		return false;
	}
	
	static void Console_Postprocess_Key( TextEdit& that )
	{
		const FSTree* key = that.GetKey();
		
		TextEditParameters& params = TextEditParameters::Get( key );
		
		ConsoleParameters& consoleParams = gConsoleParametersMap[ key ];
		
		if ( params.itsSelection.start < consoleParams.itsStartOfInput )
		{
			// Fudge 
			that.Select( consoleParams.itsStartOfInput,
			             consoleParams.itsStartOfInput );
		}
	}
	
	static bool Console_KeyDown( TextEdit& that, const EventRecord& event )
	{
		typedef const FSTree* Key;
		
		const Key viewKey = that.GetKey();
		
		TextEditParameters& params = TextEditParameters::Get( viewKey );
		
		const ConsoleParameters& consoleParams = gConsoleParametersMap[ viewKey ];
		
		Ped::TextSelection& selection = params.itsSelection;
		
		const TEHandle hTE = that.Get();
		
		ASSERT( hTE != NULL );
		
		const char c   =  event.message & charCodeMask;
		const char key = (event.message & keyCodeMask) >> 8;
		
		if ( c == kEnterCharCode  &&  key >= 0x30 )
		{
			Console_On_EnterKey( params );
			
			return true;
		}
		
		if ( that.Preprocess_Key( event ) )
		{
			return true;
		}
		
		if ( c == kReturnCharCode  ||  params.itsSelection.start < consoleParams.itsStartOfInput )
		{
			const short length = hTE[0]->teLength;
			
			selection.start =
			selection.end   = length;
			
			::TESetSelect( length, length, hTE );
		}
		
		const UInt32 kEitherControlKey = controlKey | rightControlKey;
		
		if ( Try_Control_Character( that, event ) )
		{
			return true;
		}
		
		if ( c == kBackspaceCharCode )
		{
			if ( hTE[0]->selEnd == consoleParams.itsStartOfInput )
			{
				// Eat the event -- don't backspace over the prompt.
				return true;
			}
			
			if ( event.modifiers & cmdKey )
			{
				// Don't delete the prompt.
				
				hTE[0]->selStart = consoleParams.itsStartOfInput;
			}
		}
		else if ( c == kLeftArrowCharCode  &&  params.itsSelection.start == consoleParams.itsStartOfInput )
		{
			const bool shift = event.modifiers & (shiftKey | rightShiftKey);
			
			const short cursor = shift ? params.itsSelection.start
			                           : params.itsSelection.end;
			
			if ( cursor == consoleParams.itsStartOfInput )
			{
				// Don't retreat cursor past prompt.
				return true;
			}
		}
		
		if ( that.Process_Key( event ) )
		{
			Console_Postprocess_Key( that );
			
			return true;
		}
		
		return false;
	}
	
	static boost::intrusive_ptr< Ped::View > CreateView( const FSTree* delegate )
	{
		typedef TextEdit_Scroller View;
		
		return seize_ptr( new View( delegate,
		                            Console_KeyDown,
		                            Console_UserCommand_Hook ) );
	}
	
	
	static void DestroyDelegate( const FSTree* delegate )
	{
		ScrollerParameters::Erase( delegate );
		
		TextEditParameters::Erase( delegate );
		
		gConsoleParametersMap.erase( delegate );
	}
	
	
	static FSTreePtr MakeConsoleProxy( unsigned id )
	{
		FSTreePtr parent = ResolveAbsolutePath( STR_LEN( "/dev/con" ) );
		
		plus::string name = iota::inscribe_decimal( id );
		
		return seize_ptr( new FSTree( parent, name ) );
	}
	
	class ConsoleTTYHandle : public TTYHandle
	{
		private:
			FSTreePtr  itsTTYFile;
			unsigned   itsID;
			
			const FSTree* ViewKey() const;
		
		public:
			ConsoleTTYHandle( const FSTreePtr& file, unsigned id )
			:
				TTYHandle( 0 ),
				itsTTYFile( file ),
				itsID( id )
			{
			}
			
			~ConsoleTTYHandle()
			{
				GetDynamicGroup< ConsoleTTYHandle >().erase( itsID );
			}
			
			void Attach( const boost::shared_ptr< IOHandle >& terminal );
			
			FSTreePtr GetFile()  { return MakeConsoleProxy( itsID ); }
			
			unsigned int SysPoll();
			
			ssize_t SysRead( char* data, std::size_t byteCount );
			
			ssize_t SysWrite( const char* data, std::size_t byteCount );
			
			void IOCtl( unsigned long request, int* argp );
	};
	
	void ConsoleTTYHandle::Attach( const boost::shared_ptr< IOHandle >& terminal )
	{
		const FSTree* view = ViewKey();
		
		ConsoleParameters& params = gConsoleParametersMap[ view ];
		
		params.itsTerminal = terminal;
	}
	
	const FSTree* ConsoleTTYHandle::ViewKey() const
	{
		return itsTTYFile->ParentRef().get();
	}
	
	unsigned int ConsoleTTYHandle::SysPoll()
	{
		const FSTree* view = ViewKey();
		
		ConsoleParameters& params = gConsoleParametersMap[ view ];
		
		const plus::string& s = TextEditParameters::Get( view ).its_mac_text;
		
		const bool readable = params.itsStartOfInput < s.size()  &&  *(s.end() - 1) == '\n'  ||  params.itHasReceivedEOF;
		
		int readability = readable ? kPollRead : 0;
		
		return readability | kPollWrite;
	}
	
	static void check_for_truncation( size_t               text_size,
	                                  size_t&              start_of_input,
	                                  size_t&              start_of_output,
	                                  Ped::TextSelection&  selection )
	{
		if ( start_of_input > text_size )
		{
			start_of_output =
			start_of_input  = text_size;
		}
		
		if ( selection.end > text_size )
		{
			if ( selection.start > text_size )
			{
				selection.start = text_size;
			}
			
			selection.end = text_size;
		}
	}
	
	ssize_t ConsoleTTYHandle::SysRead( char* buffer, std::size_t byteCount )
	{
		const FSTree* view = ViewKey();
		
		TextEditParameters& text_params = TextEditParameters::Get( view );
		
		ConsoleParameters& params = gConsoleParametersMap[ view ];
		
		const plus::string& s = text_params.its_mac_text;
		
		size_t command_size = 0;
		
		while ( true )
		{
			if ( params.itHasReceivedEOF )
			{
				break;
			}
			
			check_for_truncation( s.size(),
			                      params.itsStartOfInput,
			                      params.itsStartOfOutput,
			                      text_params.itsSelection );
			
			const char* p = s.c_str() + params.itsStartOfInput;
			
			if ( const char* nl = strchr( p, '\n' ) )
			{
				command_size = nl + 1 - p;
				
				break;
			}
			
			TryAgainLater();
		}
		
		if ( params.itHasReceivedEOF )
		{
			params.itHasReceivedEOF = false;
			
			return 0;
		}
		
		ASSERT( params.itsStartOfInput < s.size() );
		
		const char* begin = s.begin() + params.itsStartOfInput;
		const char* input = begin;
		
		byteCount = conv::utf8_from_mac( buffer, byteCount, &input, command_size );
		
		command_size = input - begin;
		
		params.itsStartOfOutput = 
		params.itsStartOfInput += command_size;
		
		return byteCount;
	}
	
	static inline char MacRoman_from_unicode( chars::unichar_t uc )
	{
		using chars::extended_ascii_from_unicode;
		using chars::MacRoman_encoder_map;
		
		return extended_ascii_from_unicode( uc, MacRoman_encoder_map );
	}
	
	ssize_t ConsoleTTYHandle::SysWrite( const char* buffer, std::size_t byteCount )
	{
		const FSTree* view = ViewKey();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		ConsoleParameters& consoleParams = gConsoleParametersMap[ view ];
		
		plus::var_string& s = params.its_mac_text;
		
		check_for_truncation( s.size(),
		                      consoleParams.itsStartOfInput,
		                      consoleParams.itsStartOfOutput,
		                      params.itsSelection );
		
		const size_t max_TextEdit_size = 30000;
		
		// byteCount is for UTF-8, so non-ASCII chars may cause an early cut
		
		if ( s.size() + byteCount > max_TextEdit_size )
		{
			size_t n_cut = consoleParams.itsStartOfOutput / 2;
			
			while ( n_cut > 0  &&  s[ n_cut - 1 ] != '\n' )
			{
				--n_cut;
			}
			
			s.erase( s.begin(), s.begin() + n_cut );
			
			consoleParams.itsStartOfInput  -= n_cut;
			consoleParams.itsStartOfOutput -= n_cut;
			
			params.itsValidLength = 0;
			
			if ( params.itsSelection.start >= n_cut )
			{
				params.itsSelection.start -= n_cut;
				params.itsSelection.end   -= n_cut;
			}
			else
			{
				params.itsSelection.start =
				params.itsSelection.end   = s.length();
			}
			
			const size_t max_write_after_cut = 8192;
			
			byteCount = std::min( byteCount, max_write_after_cut );
		}
		
		const size_t start_of_input  = consoleParams.itsStartOfInput;
		const size_t start_of_output = consoleParams.itsStartOfOutput;
		
		ASSERT( start_of_input <= s.size() );
		ASSERT( start_of_input >= start_of_output );
		
		params.itsValidLength = std::min( params.itsValidLength, start_of_output );
		
		const plus::string saved_input( s.begin() + start_of_input, s.end() );
		
		if ( start_of_output + byteCount > s.size() )
		{
			s.resize( start_of_output + byteCount );
		}
		
		const size_t bytes_overwritable = start_of_input - start_of_output;
		
		char *const start_of_output_p = &s[ start_of_output ];
		char *      end_of_output_p   = &s[ start_of_input  ];
		
		char* p = start_of_output_p;
		
		char* start_of_last_line = NULL;
		
		const char* buffer_end = buffer + byteCount;
		
		const char* mark = buffer;
		
		while ( mark < buffer_end )
		{
			const chars::unichar_t uc = chars::get_next_code_point_from_utf8( mark, buffer_end );
			
			char c = uc;
			
			if ( !~uc )
			{
				c = 0xC1;  // inverted exclamation mark
				
				++mark;
			}
			else if ( uc >= 0x80 )
			{
				c = MacRoman_from_unicode( uc );
				
				if ( c == '\0' )
				{
					c = 0xC0;  // inverted question mark
				}
			}
			
			switch ( c )
			{
				case 0x07:
					Mac::SysBeep();
					break;
				
				case '\r':
					if ( start_of_last_line == NULL )
					{
						start_of_last_line = &s[ 0 ];
						
						for ( int j = start_of_output - 1;  j >= 0;  --j )
						{
							if ( s[ j ] == '\n' )
							{
								start_of_last_line = &s[ j ] + 1;
								
								break;
							}
						}
					}
					
					p = start_of_last_line;
					break;
				
				case '\n':
					*end_of_output_p++ = c;
					
					start_of_last_line = p = end_of_output_p;
					break;
				
				default:
					*p++ = c;
					
					end_of_output_p = std::max( end_of_output_p, p );
					
					break;
			}
		}
		
		const size_t bytes_written = end_of_output_p - start_of_output_p;
		
		const size_t bytes_inserted = std::max( int( bytes_written - bytes_overwritable ), 0 );
		
		if ( bytes_inserted == 0 )
		{
			s.resize( start_of_input + saved_input.size() );
		}
		else
		{
			s.resize( start_of_output + bytes_written );
			
			s += saved_input;
		}
		
		params.its_utf8_text = plus::utf8_from_mac( s );
		
		if ( params.itsSelection.start >= start_of_input )
		{
			params.itsSelection.start += bytes_inserted;
			params.itsSelection.end   += bytes_inserted;
		}
		else if ( params.itsSelection.end <= start_of_input )
		{
			// preserve selection
		}
		else
		{
			params.itsSelection.start =
			params.itsSelection.end   = s.length();
		}
		
		consoleParams.itsStartOfInput  += bytes_inserted;
		consoleParams.itsStartOfOutput += p - start_of_output_p;
		
		params.itHasChangedAttributes = true;
		
		InvalidateWindowForView( view );
		
		return byteCount;
	}
	
	void ConsoleTTYHandle::IOCtl( unsigned long request, int* argp )
	{
		const FSTree* view = ViewKey();
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		Point* result = (Point*) argp;
		
		switch ( request )
		{
			case TIOCGWINSZ:
				if ( result != NULL )
				{
					const Rect& bounds = ScrollerParameters::Get( view ).itsLastViewBounds;
					
					result[0] = params.itsTextDimensions;
					result[1] = N::SetPt( bounds.right - bounds.left, bounds.bottom - bounds.top );
				}
				
				break;
			
			default:
				TTYHandle::IOCtl( request, argp );
				break;
		};
	}
	
	
	class FSTree_Console_tty : public FSTree
	{
		public:
			FSTree_Console_tty( const FSTreePtr&     parent,
			                    const plus::string&  name )
			:
				FSTree( parent, name )
			{
			}
			
			const FSTree* WindowKey() const  { return ParentRef().get(); }
			
			mode_t FileTypeMode() const  { return S_IFCHR; }
			mode_t FilePermMode() const  { return S_IRUSR | S_IWUSR; }
			
			void Rename( const FSTreePtr& destination ) const;
			
			boost::shared_ptr< IOHandle > Open( OpenFlags flags ) const;
	};
	
	void FSTree_Console_tty::Rename( const FSTreePtr& destination ) const
	{
		destination->Attach( Self() );
	}
	
	static inline boost::shared_ptr< IOHandle >
	//
	NewConsoleTTY( const FSTreePtr& self, TerminalID id )
	{
		return seize_ptr( new ConsoleTTYHandle( self, id ) );
	}
	
	boost::shared_ptr< IOHandle >
	//
	FSTree_Console_tty::Open( OpenFlags flags ) const
	{
		static unsigned gLastID = 0;
		
		unsigned id = ++gLastID;
		
		boost::shared_ptr< IOHandle > result( NewConsoleTTY( Self(), id ) );
		
		GetDynamicGroup< ConsoleTTYHandle >()[ id ] = result;
		
		return result;
	}
	
	
	template < class Serialize, typename Serialize::result_type& (*Access)( const FSTree* ) >
	struct Console_View_Property : public View_Property< Serialize, Access >
	{
		static void set( const FSTree* that, const char* begin, const char* end, bool binary )
		{
			TextEditParameters::Get( that ).itHasChangedAttributes = true;
			
			View_Property< Serialize, Access >::set( that, begin, end, binary );
		}
	};
	
	
	#define PROPERTY( prop )  &new_property, &property_params_factory< prop >::value
	
	typedef Const_View_Property< plus::serialize_bool, TextEditParameters::Active >  Active_Property;
	
	typedef View_Property< plus::serialize_bool, TextEditParameters::Wrapped >  Wrapped_Property;
	
	typedef View_Property< plus::serialize_int< int >, ScrollerParameters::Width  >  Width_Property;
	typedef View_Property< plus::serialize_int< int >, ScrollerParameters::Height >  Height_Property;
	
	typedef Console_View_Property< plus::serialize_int< int >, ScrollerParameters::HOffset >  HOffset_Property;
	typedef Console_View_Property< plus::serialize_int< int >, ScrollerParameters::VOffset >  VOffset_Property;
	
	static const FSTree_Premapped::Mapping local_mappings[] =
	{
		{ "tty", &Basic_Factory< FSTree_Console_tty > },
		
		{ "text", &New_FSTree_TextEdit_text },
		
		{ "selection", PROPERTY( Selection_Property ) },
		
		{ "active", PROPERTY( Active_Property ) },
		
		//{ "wrapped", PROPERTY( Wrapped_Property ) },
		
		// unlocked-text
		
		{ "width",  PROPERTY( Width_Property  ) },
		{ "height", PROPERTY( Height_Property ) },
		
		{ "x", PROPERTY( HOffset_Property ) },
		{ "y", PROPERTY( VOffset_Property ) },
		
		{ NULL, NULL }
	};
	
	FSTreePtr New_FSTree_new_console( const FSTreePtr&     parent,
	                                  const plus::string&  name,
	                                  const void*          args )
	{
		return seize_ptr( new FSTree_new_View( parent,
		                                       name,
		                                       &CreateView,
		                                       local_mappings,
		                                       &DestroyDelegate ) );
	}
	
}

