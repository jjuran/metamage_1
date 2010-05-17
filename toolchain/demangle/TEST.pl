#!/usr/bin/perl

use warnings;
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

die "Bad path '$exe'\n" if !shell_safe( $exe );

my $tests = << "[END]";
.main                                                    main
._lamp_main                                              _lamp_main
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
		
		print "not " 
	}
	
	print "ok " . ++$nth . "\n";
}

