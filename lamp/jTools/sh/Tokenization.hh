// ===============
// Tokenization.hh
// ===============

#ifndef SHELLSHOCK_TOKENIZATION_HH
#define SHELLSHOCK_TOKENIZATION_HH

// Standard C++
#include <vector>

// plus
#include "plus/string.hh"

// vxo-string
#include "vxo-string/strvec.hh"


typedef vxo::StrVec StringVector;

inline
StringVector MakeVector( const plus::string& s )
{
	return StringVector( s );
}

namespace ShellShock
{
	
	enum ControlOperator
	{
		kControlNone,
		kControlPipe,
		kControlBackground,
		kControlSequential,
		kControlOr,
		kControlAnd,
		kControlEndCase,
		kControlOpenSubshell,
		kControlCloseSubshell
	};
	
	enum RedirectionOperator
	{
		kRedirectNone,
		kRedirectInput,
		kRedirectInputHere,
		kRedirectInputHereStrippingTabs,
		kRedirectInputDuplicate,
		kRedirectInputAndOutput,
		kRedirectOutput,
		kRedirectOutputClobbering,
		kRedirectOutputAppending,
		kRedirectOutputDuplicate,
		kRedirectOutputAndError
	};
	
	struct ControlInfo
	{
		ControlOperator op;
		unsigned char len;
		
		ControlInfo() : op(), len()  {}
		
		ControlInfo( ControlOperator op, unsigned char len )
		:
			op ( op  ),
			len( len )
		{}
	};
	
	struct RedirectInfo
	{
		RedirectionOperator op;
		int fd;
		
		RedirectInfo() : op(), fd( -1 )  {}
		
		RedirectInfo( RedirectionOperator op, int fd = -1 )
		:
			op( op ),
			fd( fd )
		{}
	};
	
	struct RedirectInfoWithLength : RedirectInfo
	{
		unsigned char len;
		
		RedirectInfoWithLength() : len()  {}
		
		RedirectInfoWithLength( const RedirectInfo& info, unsigned char len )
		:
			RedirectInfo( info ),
			len         ( len  )
		{}
	};
	
	struct Redirection : RedirectInfo
	{
		plus::string param;
		
		Redirection()  {}
		
		Redirection( const RedirectInfo& info, const plus::string& param )
		:
			RedirectInfo( info  ),
			param       ( param )
		{}
	};
	
	struct Command
	{
		StringVector                args;
		std::vector< Redirection  > redirections;
		
		Command()  {}
		
		Command( const StringVector&                 args,
		         const std::vector< Redirection  >&  redirections )
		:
			args        ( args         ),
			redirections( redirections )
		{}
	};
	
	struct Pipeline
	{
		std::vector< Command > commands;
		ControlOperator op;  // May be kControlOr, kControlAnd, or kControlNone.
		
		Pipeline()  {}
		
		Pipeline( const std::vector< Command >&  commands,
		          ControlOperator                op = kControlNone )
		:
			commands( commands ),
			op      ( op       )
		{}
	};
	
	struct Circuit
	{
		std::vector< Pipeline > pipelines;
		ControlOperator op;  // May be kControlBackground, kControlSequential, or kControlNone.
		
		Circuit()  {}
		Circuit( const std::vector< Pipeline >&  pipelines,
		         ControlOperator                 op = kControlNone )
		:
			pipelines( pipelines ),
			op       ( op        )
		{}
	};
	
	typedef std::vector< Circuit > List;
	
	List Tokenization( const plus::string& cmdLine );
	
}

#endif
