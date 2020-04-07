package Compile::Driver::Job::Compile;

use     Compile::Driver::Job ;
@ISA = 'Compile::Driver::Job';

use Compile::Driver::InputFile::CPPSource;

use warnings FATAL => 'all';
use strict;


*read_cpp_source_file = \&Compile::Driver::InputFile::CPPSource::read_file;


sub new
{
	my $class = shift;
	
	my %self = @_;
	
	return bless \%self, $class;
}

my %includes_cache;

sub get_includes
{
	my ( $path ) = @_;
	
	exists $includes_cache{ $path } and return $includes_cache{ $path };
	
	my $includes = read_cpp_source_file( $path );
	
	return $includes_cache{ $path } = $includes;
}

sub up_to_date_for_headers
{
	my ( $module, $out_date, @headers ) = @_;
	
	foreach my $h ( @headers )
	{
		my @stat = stat $h;
		
		return 0  if !@stat || $stat[9] >= $out_date;
		
		my $includes = get_includes( $h );
		
		$includes = $includes->{USER};
		
		my @headers = grep { defined } map { $module->find_include( $_ ) } @$includes;
		
		return 0  if !up_to_date_for_headers( $module, $out_date, @headers );
	}
	
	return 1;
}

sub input_files
{
	my $self = shift;
	
	my $path = $self->{PATH};
	
	return $path;
}

sub up_to_date
{
	my $self = shift;
	
	my $out = $self->{DEST};
	
	-f $out or return 0;
	
	my $out_date = (stat _)[9];
	
	my $module = $self->{FROM};
	
	my $path = $self->{PATH};
	
	return up_to_date_for_headers( $module, $out_date, $path );
}

sub tool_name
{
	return "cc";
}

sub command
{
	my $self = shift;
	
	my $module = $self->{FROM};
	
	my $dest = $self->{DEST};
	
	my $conf = $module->{CONF};
	
	my @arch = $conf->arch_option;
	
	my @o = -O2;
	
	my @f;
	my @w;
	
	my %d;
	
	$d{ CONFIG_DEBUGGING } = $conf->debugging + 0;
	
	if ( $conf->symbolics )
	{
		# Don't turn off optimizations here.  We need them to strip dead code,
		# because without that we get link errors.
		
		push @o, '-g';
	}
	
	if ( $conf->is_carbon )
	{
		$d{ TARGET_API_MAC_CARBON } = 1;
	}
	
	if ( $conf->is_apple_gcc )
	{
		push @f, "-fpascal-strings";
		push @w, "-Wno-deprecated-declarations";
		
		push @w, "-Wno-long-double"  if ($conf->{arch} || "") eq "ppc";
		
		$d{ MAC_OS_X_VERSION_MIN_REQUIRED } = 'MAC_OS_X_VERSION_10_2';
	}
	
	my @d = map { "-D$_=" . $d{ $_ } } keys %d;
	
	my @i = map { "-I$_" } @{ $module->all_search_dirs };
	
	return $self->tool_name, '-c', -o => $dest, @arch, @o, @f, @w, @d, @i;
}

1;
