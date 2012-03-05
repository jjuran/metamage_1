package Compile::Driver::Configuration;

use Compile::Driver::Catalog;
use Compile::Driver::Module;
use Compile::Driver::Platform;

use warnings;
use strict;


my $build = "dbg";


sub new
{
	my $class = shift;
	
	my %self;
	
	$self{ unix } = "unix";
	
	$self{ build } = $build;
	
	return bless \%self, $class;
}

sub platform_mask
{
	my $self = shift;
	
	my %platform = %$self;
	
	delete $platform{ build };
	
	return Compile::Driver::Platform::mask_for_values( values %platform );
}

sub target
{
	my $self = shift;
	
	my $build = $self->{build};
	
	return $build;
}

my %module_cache;

sub get_module
{
	my $self = shift;
	
	my ( $name, $mandatory ) = @_;
	
	my $optional = !$mandatory  &&  $name =~ m{^ _ }x;
	
	my $mask = $self->platform_mask;
	
	my $key = "$name $mask";
	
	return $module_cache{ $key }  if exists $module_cache{ $key };
	
	my $desc = Compile::Driver::Catalog::find_project( $name, $mask );
	
	if ( !$desc )
	{
		if ( $optional )
		{
			$desc = { NAME => $name, MEMO => { PREREQS => [] } };
		}
		else
		{
			die "No such project '$name'\n";
		}
	}
	
	return $module_cache{ $key } = Compile::Driver::Module::->new( $self, $desc );
}

1;

