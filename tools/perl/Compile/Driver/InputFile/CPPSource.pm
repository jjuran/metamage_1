package Compile::Driver::InputFile::CPPSource;

use warnings;
use strict;


sub read_line
{
	my ( $line, $includes ) = @_;
	
	if ( $line =~ s{^ \s* \# \s* include \s+ }{}x )
	{
		if ( $line =~ m{^ " (.*) " $}x )
		{
			push @{ $includes->{USER} }, $1;
		}
		elsif ( $line =~ m{^ < (.*) > $}x )
		{
			push @{ $includes->{SYSTEM} }, $1;
		}
	}
}

sub read_file
{
	my ( $path ) = @_;
	
	open my $in, "<", $path or die "$0: $path: $!\n";
	
	my $includes = { USER => [], SYSTEM => [] };
	
	while ( my $line = <$in> )
	{
		read_line( $line, $includes );
	}
	
	return $includes;
}

1;

