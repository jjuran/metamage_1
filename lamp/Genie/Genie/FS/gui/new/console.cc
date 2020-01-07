/*
	gui/new/console.cc
	------------------
*/

#include "Genie/FS/gui/new/console.hh"

// Mac OS X
#ifdef __APPLE__
#include <Carbon/Carbon.h>
#endif

// Mac OS
#ifndef __EVENTS__
#include <Events.h>
#endif

// POSIX
#include <signal.h>
#include <sys/stat.h>
#include <sys/ttycom.h>

// Iota
#include "iota/strings.hh"

// gear
#include "gear/inscribe_decimal.hh"

// chars
#include "charsets/extended_ascii.hh"
#include "charsets/MacRoman.hh"
#include "conv/mac_utf8.hh"
#include "encoding/utf8.hh"

// plus
#include "plus/mac_utf8.hh"
#include "plus/serialize.hh"
#include "plus/simple_map.hh"
#include "plus/var_string.hh"

// poseven
#include "poseven/types/errno_t.hh"

// Nitrogen
#include "Mac/Sound/Functions/SysBeep.hh"

// vfs
#include "vfs/filehandle.hh"
#include "vfs/file_descriptor.hh"
#include "vfs/node.hh"
#include "vfs/enum/poll_result.hh"
#include "vfs/filehandle/functions/nonblocking.hh"
#include "vfs/filehandle/methods/filehandle_method_set.hh"
#include "vfs/filehandle/methods/general_method_set.hh"
#include "vfs/filehandle/methods/stream_method_set.hh"
#include "vfs/filehandle/primitives/getpgrp.hh"
#include "vfs/filehandle/types/dynamic_group.hh"
#include "vfs/functions/resolve_pathname.hh"
#include "vfs/methods/data_method_set.hh"
#include "vfs/methods/item_method_set.hh"
#include "vfs/methods/node_method_set.hh"
#include "vfs/node/types/property_file.hh"
#include "vfs/primitives/attach.hh"

// relix
#include "relix/api/root.hh"
#include "relix/api/try_again.hh"
#include "relix/fs/con_group.hh"
#include "relix/signal/signal_process_group.hh"

// Genie
#include "Genie/ProcessList.hh"
#include "Genie/FS/TextEdit.hh"
#include "Genie/FS/TextEdit_text.hh"
#include "Genie/FS/Views.hh"


namespace Genie
{
	
	namespace p7 = poseven;
	namespace Ped = Pedestal;
	
	
	template < class T >
	static inline T min( T a, T b )
	{
		return b < a ? b : a;
	}
	
	template < class T >
	static inline T max( T a, T b )
	{
		return b < a ? a : b;
	}
	
	static void RunShellCommand( const plus::string& command )
	{
		const char* argv[] = { "-sh", "-c", "", NULL };
		
		argv[2] = command.c_str();
		
		spawn_process( "/bin/sh", argv );
	}
	
	
	struct ConsoleParameters
	{
		vfs::filehandle_ptr  itsTerminal;
		std::size_t          itsStartOfInput;
		std::size_t          itsStartOfOutput;
		bool                 itHasReceivedEOF;
		
		ConsoleParameters()
		:
			itsStartOfInput(),
			itsStartOfOutput(),
			itHasReceivedEOF()
		{
		}
	};
	
	typedef plus::simple_map< const vfs::node*, ConsoleParameters > ConsoleParametersMap;
	
	static ConsoleParametersMap gConsoleParametersMap;
	
	
	static bool Console_UserCommand_Hook( TextEdit& that, Ped::CommandCode code )
	{
		bool handled = false;
		
		const vfs::node* key = that.GetKey();
		
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
	
	static void SendSignalToProcessGroupForKey( int signo, const vfs::node* key )
	{
		ConsoleParameters& params = gConsoleParametersMap[ key ];
		
		const pid_t pgid = getpgrp( *params.itsTerminal );
		
		relix::signal_process_group( signo, pgid );
	}
	
	static bool Try_Control_Character( TextEdit& that, const EventRecord& event )
	{
		const UInt32 kEitherControlKey = controlKey | rightControlKey;
		
		const char c = event.message & charCodeMask;
		
		const UInt32 kPrimaryModifiers = cmdKey
		                               | shiftKey   | rightShiftKey
		                               | optionKey  | rightOptionKey
		                               | controlKey | rightControlKey;
		
		if ( (event.modifiers & kPrimaryModifiers) == cmdKey  &&  c == '.' )
		{
			SendSignalToProcessGroupForKey( SIGINT, that.GetKey() );
			
			return true;
		}
		else if ( event.modifiers & kEitherControlKey  &&  c < 0x20 )
		{
			typedef const vfs::node* Key;
			
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
		const vfs::node* key = that.GetKey();
		
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
		typedef const vfs::node* Key;
		
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
	
	static boost::intrusive_ptr< Ped::View > CreateView( const vfs::node* delegate )
	{
		typedef TextEdit_Scroller View;
		
		return new View( delegate,
		                 Console_KeyDown,
		                 Console_UserCommand_Hook );
	}
	
	
	static void DestroyDelegate( const vfs::node* delegate )
	{
		ScrollerParameters::Erase( delegate );
		
		TextEditParameters::Erase( delegate );
		
		gConsoleParametersMap.erase( delegate );
	}
	
	
	struct console_extra
	{
		vfs::node const*  tty_file;
		unsigned          id;
	};
	
	static
	const vfs::node* console_view_key( const vfs::filehandle* that );
	
	static
	void destroy_console( vfs::filehandle* that )
	{
		console_extra& extra = *(console_extra*) that->extra();
		
		relix::get_con_group().erase( extra.id );
		
		intrusive_ptr_release( extra.tty_file );
	}
	
	
	static
	unsigned consoletty_poll( vfs::filehandle* that );
	
	static
	ssize_t consoletty_read( vfs::filehandle* that, char* buffer, size_t n );
	
	static
	ssize_t consoletty_write( vfs::filehandle* that, const char* buffer, size_t n );
	
	static
	void consoletty_ioctl( vfs::filehandle* that, unsigned long req, int* arg );
	
	static
	void consoletty_conjoin( vfs::filehandle& that, vfs::filehandle& target );
	
	static const vfs::stream_method_set consoletty_stream_methods =
	{
		&consoletty_poll,
		&consoletty_read,
		&consoletty_write,
	};
	
	static const vfs::general_method_set consoletty_general_methods =
	{
		NULL,
		&consoletty_ioctl,
		NULL,
		&consoletty_conjoin,
	};
	
	static const vfs::filehandle_method_set consoletty_methods =
	{
		NULL,
		NULL,
		&consoletty_stream_methods,
		&consoletty_general_methods,
	};
	
	
	static
	vfs::filehandle* new_tty_handle( const vfs::node& file, unsigned id )
	{
		vfs::filehandle* result = new vfs::filehandle( 0,
		                                               &consoletty_methods,
		                                               sizeof (console_extra),
		                                               &destroy_console );
		
		console_extra& extra = *(console_extra*) result->extra();
		
		extra.id = id;
		extra.tty_file = &file;
		
		intrusive_ptr_add_ref( &file );
		
		return result;
	}
	
	static
	void consoletty_conjoin( vfs::filehandle& that, vfs::filehandle& target )
	{
		const vfs::node* view = console_view_key( &that );
		
		ConsoleParameters& params = gConsoleParametersMap[ view ];
		
		params.itsTerminal = &target;
	}
	
	static
	const vfs::node* console_view_key( const vfs::filehandle* that )
	{
		console_extra& extra = *(console_extra*) that->extra();
		
		return extra.tty_file->owner();
	}
	
	static
	unsigned consoletty_poll( vfs::filehandle* that )
	{
		const vfs::node* view = console_view_key( that );
		
		ConsoleParameters& params = gConsoleParametersMap[ view ];
		
		const plus::string& s = TextEditParameters::Get( view ).its_mac_text;
		
		const size_t size = s.size();
		
		const bool readable = (params.itsStartOfInput < size  &&  s[ size - 1 ] == '\n')  ||  params.itHasReceivedEOF;
		
		int readability = readable ? vfs::Poll_read : 0;
		
		return readability | vfs::Poll_write;
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
	
	static
	ssize_t consoletty_read( vfs::filehandle* that, char* buffer, size_t n )
	{
		const vfs::node* view = console_view_key( that );
		
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
			
			relix::try_again( is_nonblocking( *that ) );
		}
		
		if ( params.itHasReceivedEOF )
		{
			params.itHasReceivedEOF = false;
			
			return 0;
		}
		
		ASSERT( params.itsStartOfInput < s.size() );
		
		const char* begin = s.begin() + params.itsStartOfInput;
		const char* input = begin;
		
		n = conv::utf8_from_mac( buffer, n, &input, command_size );
		
		command_size = input - begin;
		
		params.itsStartOfOutput = 
		params.itsStartOfInput += command_size;
		
		text_params.itsSelection.start =
		text_params.itsSelection.end   = s.size();
		
		return n;
	}
	
	static inline char MacRoman_from_unicode( chars::unichar_t uc )
	{
		using chars::extended_ascii_from_unicode;
		using chars::MacRoman_encoder_map;
		
		return extended_ascii_from_unicode( uc, MacRoman_encoder_map );
	}
	
	static
	ssize_t consoletty_write( vfs::filehandle* that, const char* buffer, size_t n )
	{
		const vfs::node* view = console_view_key( that );
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		ConsoleParameters& consoleParams = gConsoleParametersMap[ view ];
		
		plus::var_string& s = params.its_mac_text;
		
		check_for_truncation( s.size(),
		                      consoleParams.itsStartOfInput,
		                      consoleParams.itsStartOfOutput,
		                      params.itsSelection );
		
		const size_t max_TextEdit_size = 30000;
		
		// n is for UTF-8, so non-ASCII chars may cause an early cut
		
		if ( s.size() + n > max_TextEdit_size )
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
			
			n = min( n, max_write_after_cut );
		}
		
		const size_t start_of_input  = consoleParams.itsStartOfInput;
		const size_t start_of_output = consoleParams.itsStartOfOutput;
		
		ASSERT( start_of_input <= s.size() );
		ASSERT( start_of_input >= start_of_output );
		
		params.itsValidLength = min( params.itsValidLength, start_of_output );
		
		const plus::string saved_input( s.begin() + start_of_input, s.end() );
		
		if ( start_of_output + n > s.size() )
		{
			s.resize( start_of_output + n );
		}
		
		const size_t bytes_overwritable = start_of_input - start_of_output;
		
		char *const start_of_output_p = &s[ start_of_output ];
		char *      end_of_output_p   = &s[ start_of_input  ];
		
		char* p = start_of_output_p;
		
		char* start_of_last_line = NULL;
		
		const char* buffer_end = buffer + n;
		
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
						
						size_t j;
						
						for ( j = start_of_output;  j > 0; )
						{
							--j;
							
							if ( s[ j ] == '\n' )
							{
								start_of_last_line = &s[ j ] + 1;
								
								break;
							}
						}
						
						params.itsValidLength = min( params.itsValidLength, j );
					}
					
					p = start_of_last_line;
					break;
				
				case '\n':
					*end_of_output_p++ = c;
					
					start_of_last_line = p = end_of_output_p;
					break;
				
				default:
					*p++ = c;
					
					end_of_output_p = max( end_of_output_p, p );
					
					break;
			}
		}
		
		const size_t bytes_written = end_of_output_p - start_of_output_p;
		
		const size_t bytes_inserted = max( int( bytes_written - bytes_overwritable ), 0 );
		
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
		
		return n;
	}
	
	static
	void consoletty_ioctl( vfs::filehandle* that, unsigned long req, int* argp )
	{
		const vfs::node* view = console_view_key( that );
		
		TextEditParameters& params = TextEditParameters::Get( view );
		
		Point* result = (Point*) argp;
		
		switch ( req )
		{
			case TIOCGWINSZ:
				if ( result != NULL )
				{
					const Rect& bounds = ScrollerParameters::Get( view ).itsLastViewBounds;
					
					const short width  = bounds.right - bounds.left;
					const short height = bounds.bottom - bounds.top;
					
					result[ 0 ]   = params.itsTextDimensions;
					result[ 1 ].v = height;
					result[ 1 ].h = width;
				}
				
				break;
			
			default:
				p7::throw_errno( EINVAL );
				
				break;
		};
	}
	
	
	static void console_tty_rename( const vfs::node* that, const vfs::node* destination )
	{
		attach( *destination, *that );
	}
	
	static vfs::filehandle_ptr console_tty_open( const vfs::node* that, int flags, mode_t mode )
	{
		static unsigned gLastID = 0;
		
		unsigned id = ++gLastID;
		
		vfs::filehandle_ptr result( new_tty_handle( *that, id ) );
		
		vfs::dynamic_group& group = relix::get_con_group();
		
		vfs::set_dynamic_element_of_group_by_id( group, id, result.get() );
		
		plus::var_string path = "/dev/con/";
		
		path += gear::inscribe_unsigned_decimal( id );
		
		result->set_file( *vfs::resolve_absolute_path( *relix::root(), path ) );
		
		return result;
	}
	
	static const vfs::item_method_set console_tty_item_methods =
	{
		NULL,
		NULL,
		NULL,
		NULL,
		NULL,
		&console_tty_rename,
	};
	
	static const vfs::data_method_set console_tty_data_methods =
	{
		&console_tty_open
	};
	
	static const vfs::node_method_set console_tty_methods =
	{
		&console_tty_item_methods,
		&console_tty_data_methods
	};
	
	static vfs::node_ptr console_tty_factory( const vfs::node*     parent,
	                                          const plus::string&  name,
	                                          const void*          args )
	{
		return new vfs::node( parent, name, S_IFCHR | 0600, &console_tty_methods );
	}
	
	
	template < class Serialize, typename Serialize::result_type& (*Access)( const vfs::node* ) >
	struct Console_View_Property : public View_Property< Serialize, Access >
	{
		static void set( const vfs::node* that, const char* begin, const char* end, bool binary )
		{
			TextEditParameters::Get( that ).itHasChangedAttributes = true;
			
			View_Property< Serialize, Access >::set( that, begin, end, binary );
		}
	};
	
	
	#define PROPERTY( prop )  &vfs::new_property, &vfs::property_params_factory< prop >::value
	
	typedef Const_View_Property< plus::serialize_bool, TextEditParameters::Active >  Active_Property;
	
	typedef Console_View_Property< plus::serialize_int< int >, ScrollerParameters::HOffset >  HOffset_Property;
	typedef Console_View_Property< plus::serialize_int< int >, ScrollerParameters::VOffset >  VOffset_Property;
	
	static const vfs::fixed_mapping local_mappings[] =
	{
		{ "tty", &console_tty_factory },
		
		{ "text", &New_FSTree_TextEdit_text },
		
		{ "selection", PROPERTY( Selection_Property ) },
		
		{ "active", PROPERTY( Active_Property ) },
		
		{ "width",  &vfs::new_property, &scroller_setting_params },
		{ "height", &vfs::new_property, &scroller_setting_params },
		
		{ "x", PROPERTY( HOffset_Property ) },
		{ "y", PROPERTY( VOffset_Property ) },
		
		{ NULL, NULL }
	};
	
	vfs::node_ptr New_FSTree_new_console( const vfs::node*     parent,
	                                      const plus::string&  name,
	                                      const void*          args )
	{
		return New_new_view( parent,
		                     name,
		                     &CreateView,
		                     local_mappings,
		                     &DestroyDelegate );
	}
	
}
