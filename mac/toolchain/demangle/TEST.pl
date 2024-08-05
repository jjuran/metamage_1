#!/usr/bin/perl

use warnings FATAL => 'all';
use strict;

sub shell_safe
{
	my ( $path ) = @_;
	
	return $path =~ m{^ [\w/.-]+ $}x;
}

sub escape_shell_metachars
{
	s{ ([^ \w / . , - ]) }{\\$1}gx;
}

my $exe = shift;

$exe = "demangle" if !defined $exe;

die "Bad path '$exe'\n" if !shell_safe( $exe );

my $tests = << "[END]";
.main                                                    main
._relix_main                                             _relix_main
.__sinit                                                 __sinit
.__sinit_get_options_cc                                  __sinit_get_options_cc
.__end__catch                                            __end__catch
.__throw_bad_alloc__3stdFv                               std::__throw_bad_alloc( void )
.locate_symbol_name__6recallFPCv                         recall::locate_symbol_name( void const* )
.__ct__Q26recall17debugging_contextFv                    recall::debugging_context::debugging_context( void )
.__dt__Q27nucleus30error_code<Q27poseven7errno_t>Fv      nucleus::error_code< poseven::errno_t >::~errno_t >( void )
.__cl__Q27nucleus31converter<Q27poseven7errno_t,i>CFRCi  nucleus::converter< poseven::errno_t, int >::operator()( int const& ) const
.pure_magnitude<10,Ui>__4iotaFUi                         iota::pure_magnitude< 10, unsigned int >( unsigned int )
.__nw__FUl                                               operator new( unsigned long )
.__dl__FPv                                               operator delete( void* )
.__nwa__FUl                                              operator new[]( unsigned long )
.__dla__FPv                                              operator delete[]( void* )
[END]

my $more_tests = << "[END]";
.AEInstallObjectAccessor<&.Accessor__Q27UseEdit16Document_ElementFQ23Mac13AEObjectClassRCQ23Mac12AEDesc_TokenQ23Mac13AEObjectClassQ23Mac9AEKeyFormRCQ23Mac11AEDesc_Data>__8NitrogenFQ23Mac13AEObjectClassQ23Mac8DescTypeb
Nitrogen::AEInstallObjectAccessor< UseEdit::Document_Element::Accessor( Mac::AEObjectClass, Mac::AEDesc_Token const&, Mac::AEObjectClass, Mac::AEKeyForm, Mac::AEDesc_Data const& ) >( Mac::AEObjectClass, Mac::DescType, bool )

.AEInstallObjectAccessor<v,&.Accessor__Q27UseEdit16Document_ElementFQ23Mac13AEObjectClassRCQ23Mac12AEDesc_TokenQ23Mac13AEObjectClassQ23Mac9AEKeyFormRCQ23Mac11AEDesc_Data>__8NitrogenFQ23Mac13AEObjectClassQ23Mac8DescTypeb
Nitrogen::AEInstallObjectAccessor< void, UseEdit::Document_Element::Accessor( Mac::AEObjectClass, Mac::AEDesc_Token const&, Mac::AEObjectClass, Mac::AEKeyForm, Mac::AEDesc_Data const& ) >( Mac::AEObjectClass, Mac::DescType, bool )

.AEInstallEventHandler<&.Adapter__Q28Nitrogen110AEEventHandler_Callback<v,&.Handler__Q27UseEdit18GetData_AppleEventFRCQ23Mac11AEDesc_DataRQ23Mac11AEDesc_Data>FPC6AEDescP6AEDescl,PCv>__8NitrogenFQ23Mac12AEEventClassQ23Mac9AEEventIDPCvb
Nitrogen::AEInstallEventHandler< Nitrogen::AEEventHandler_Callback< void, UseEdit::GetData_AppleEvent::Handler( Mac::AEDesc_Data const&, Mac::AEDesc_Data& ) >::Adapter( AEDesc const*, AEDesc*, long ), void const* >( Mac::AEEventClass, Mac::AEEventID, void const*, bool )
[END]

$tests .= join "\n", map { join " ", /^ (.*) \n (.*) $/x } grep { m{ \S }x } split m{ \n\n }x, $more_tests;

my @tests = grep { m{ \S }x } split m{ \n }x, $tests;

my $n_tests = @tests;

my $nth = 0;

print "1..$n_tests\n";

foreach my $test ( @tests )
{
	my ( $symbol, $pretty ) = $test =~ m{^ (\S+) \s+ (.+) }x;
	
	escape_shell_metachars for $symbol;
	
	my $cmd = "$exe $symbol";
	
	my $got = `$cmd 2>&1`;
	
	$got ||= "\n";
	
	if ( $got ne "$pretty\n" )
	{
		print "# EXPECTED: $pretty\n";
		print "# RECEIVED: $got";
		
		print "\n" . "not ";
	}
	
	print "ok " . ++$nth . "\n";
}
