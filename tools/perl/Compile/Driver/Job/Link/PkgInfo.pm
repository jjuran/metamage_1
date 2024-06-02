package Compile::Driver::Job::Link::PkgInfo;

use     Compile::Driver::Job ;
@ISA = 'Compile::Driver::Job';

use Compile::Driver::Files;

use warnings FATAL => 'all';
use strict;


*make_ancestor_dirs = \&Compile::Driver::Files::make_ancestor_dirs;


sub splat
{
	my ( $path, $data ) = @_;
	
	open my $out, ">", $path or die "$path: $!\n";
	
	print $out $data or die "$path: $!\n";
	
	close $out or die "$path: $!\n";
}

sub new
{
	my $class = shift;
	
	my %self = @_;
	
	return bless \%self, $class;
}

sub unquote
{
	my ( $code ) = @_;
	
	for ( $code )
	{
		return + /^....$/     ? $code
		       : /^'(....)'$/ ? $1
		       :                die "bad creator code";
	}
}

sub perform
{
	my $self = shift;
	
	$self->print;
	
	my $module = $self->{FROM};
	
	my $dest = $self->{DEST};
	
	make_ancestor_dirs( $dest );
	
	splat( $dest, "APPL" . unquote $module->mac_creator );
}

1;
