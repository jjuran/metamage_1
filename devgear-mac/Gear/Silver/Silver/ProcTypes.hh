/*
	
	ProcTypes.hh
	
	Joshua Juran
	
*/

#ifndef SILVER_PROCTYPES_HH
#define SILVER_PROCTYPES_HH


namespace Silver
{
	
	template < class Result >
	struct ProcType0
	{
		typedef pascal Result ( *ProcPtr )();
		typedef        Result ( *CProcPtr )();
	};
	
	template < class Result, class Arg1 >
	struct ProcType1
	{
		typedef pascal Result ( *ProcPtr )( Arg1 );
		typedef        Result ( *CProcPtr )( Arg1 );
	};
	
	template < class Result, class Arg1, class Arg2 >
	struct ProcType2
	{
		typedef pascal Result ( *ProcPtr)( Arg1, Arg2 );
		typedef        Result ( *CProcPtr)( Arg1, Arg2 );
	};
	
	template < class Result, class Arg1, class Arg2, class Arg3 >
	struct ProcType3
	{
		typedef pascal Result ( *ProcPtr)( Arg1, Arg2, Arg3 );
		typedef        Result ( *CProcPtr)( Arg1, Arg2, Arg3 );
	};
	
	template < class Result, class Arg1, class Arg2, class Arg3, class Arg4 >
	struct ProcType4
	{
		typedef pascal Result ( *ProcPtr)( Arg1, Arg2, Arg3, Arg4 );
		typedef        Result ( *CProcPtr)( Arg1, Arg2, Arg3, Arg4 );
	};
	
	template < class R, class A1, class A2, class A3, class A4, class A5 >
	struct ProcType5
	{
		typedef pascal R ( *ProcPtr )( A1, A2, A3, A4, A5 );
		typedef        R ( *CProcPtr)( A1, A2, A3, A4, A5 );
	};
	
	template < class R, class A1, class A2, class A3, class A4, class A5, class A6 >
	struct ProcType6
	{
		typedef pascal R ( *ProcPtr )( A1, A2, A3, A4, A5, A6 );
		typedef        R ( *CProcPtr)( A1, A2, A3, A4, A5, A6 );
	};
	
}

#endif

